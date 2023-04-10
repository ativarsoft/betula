static SIGMA: &'static str = "expand 32-byte k";
static TAU: &'static str = "expand 16-byte k";

static TEST1_EXPECTED_STATE: [u32; 16] =
    [
        0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
    ];

static TEST1_EXPECTED_CYPHERTEXT: [u8; 8 * 8] =
    [
        0x76, 0xb8, 0xe0, 0xad, 0xa0, 0xf1, 0x3d, 0x90,
        0x40, 0x5d, 0x6a, 0xe5, 0x53, 0x86, 0xbd, 0x28,
        0xbd, 0xd2, 0x19, 0xb8, 0xa0, 0x8d, 0xed, 0x1a,
        0xa8, 0x36, 0xef, 0xcc, 0x8b, 0x77, 0x0d, 0xc7,
        0xda, 0x41, 0x59, 0x7c, 0x51, 0x57, 0x48, 0x8d,
        0x77, 0x24, 0xe0, 0x3f, 0xb8, 0xd8, 0x4a, 0x37,
        0x6a, 0x43, 0xb8, 0xf4, 0x15, 0x18, 0xa1, 0x1c,
        0xc3, 0x87, 0xb6, 0x69, 0xb2, 0xee, 0x65, 0x86,
     ];

pub mod chacha20 {

    pub type Allocator = extern "C" fn(bytes: usize) -> *mut u8;
    pub type Free = extern "C" fn(*mut u8);

    //
    // Utility functions
    //

    fn u32_to_u8_little(value: u32) -> [u8;4] {
        let mut data: [u8;4] = [0;4];
        data[0] = (value >> (0 * 8) & 0xFF) as u8;
        data[1] = (value >> (1 * 8) & 0xFF) as u8;
        data[2] = (value >> (2 * 8) & 0xFF) as u8;
        data[3] = (value >> (3 * 8) & 0xFF) as u8;
        return data;
    }

    fn u8_to_u32_little(data: &[u8]) -> u32 {
        let mut value: u32 = 0;
        value = value | ((data[0] as u32) << (0 * 8));
        value = value | ((data[1] as u32) << (1 * 8));
        value = value | ((data[2] as u32) << (2 * 8));
        value = value | ((data[3] as u32) << (3 * 8));
        return value;
    }

    fn rotate_left(value: u32, bits: u32) -> u32 {
        let rotated_value = (value << bits) | (value >> (32 - bits));
        return rotated_value;
    }

    //
    // Encryption / Decryption
    //

    fn quarter_round(x: [u32;16], a: usize, b: usize, c: usize, d: usize) -> [u32;16] {
        let mut x: [u32;16] = x; // Create an array with the same name, but mutable
        x[a] = ((x[a] as u64 + x[b] as u64) & 0xffffffff) as u32;
        x[d] = rotate_left(x[d] ^ x[a], 16);
        x[c] = ((x[c] as u64 + x[d as usize] as u64) & 0xffffffff) as u32;
        x[b] = rotate_left(x[b] ^ x[c], 12);
        x[a] = ((x[a] as u64 + x[b as usize] as u64) & 0xffffffff) as u32;
        x[d] = rotate_left(x[d] ^ x[a], 8);
        x[c] = ((x[c] as u64 + x[d] as u64) & 0xffffffff) as u32;
        x[b] = rotate_left(x[b] ^ x[c], 7);
        return x;
    }

    fn salsa20_word_to_byte(input: [u32; 16]) -> [u8;64] {
        let mut x = input;
        let mut i = 0;
        while i < 10 {
            // "column" round
            x = quarter_round(x, 0, 4, 8,  12);
            x = quarter_round(x, 1, 5, 9,  13);
            x = quarter_round(x, 2, 6, 10, 14);
            x = quarter_round(x, 3, 7, 11, 15);
            // "diagonal" round
            x = quarter_round(x, 0, 5, 10, 15);
            x = quarter_round(x, 1, 6, 11, 12);
            x = quarter_round(x, 2, 7,  8, 13);
            x = quarter_round(x, 3, 4,  9, 14);
            i = i + 1;
        }
        for i in 0 .. input.len() {
            x[i as usize] =
                ((x[i as usize] as u64 +
                input[i as usize] as u64) &
                0xffffffff) as u32;
        }
        let mut output: [u8;64] = [0;64];
        for i in 0 .. input.len() {
            let first: usize = 4 * i;
            let buffer = u32_to_u8_little(x[i as usize]);
            output[first + 0] = buffer[0];
            output[first + 1] = buffer[1];
            output[first + 2] = buffer[2];
            output[first + 3] = buffer[3];
        }
        return output;
    }

    pub struct ChaCha20 {
        malloc: Allocator,
        free: Free,
        input: [u32; 16],
    }

    impl ChaCha20 {
        pub fn dump_state(&self) -> [u32; 16] {
            return self.input;
        }

        pub fn encrypt_bytes(&mut self, m: &mut [u8], c: &mut [u8], bytes: usize) {
            if bytes == 0 {
                return;
            }
            let c_first = 0;
            let m_first = 0;
            let mut c_index: usize = c_first;
            let mut m_index: usize = m_first;
            let mut remaining_bytes: usize = bytes;
            loop {
                let output: [u8; 64] = salsa20_word_to_byte(self.input);
                self.input[12] = self.input[12] + 1;
                if self.input[12] == 0 {
                    self.input[13] = self.input[13] + 1;
                    // stopping at 2^70 bytes per nonce is user's
                    // responsibility
                }
                if remaining_bytes <= 64 {
                    for j in 0 .. remaining_bytes {
                        c[j + c_index] = m[j + m_index] ^ output[j];
                    }
                    return;
                }
                for j in 0 .. 64 {
                    c[j + c_index] = m[j + m_index] ^ output[j];
                }
                remaining_bytes -= 64;
                c_index += 64;
                m_index += 64;
            }
        }

        pub fn decrypt_bytes(&mut self, m: &mut [u8], c: &mut [u8], bytes: usize) {
            self.encrypt_bytes(c, m, bytes);
        }

        pub fn keystream_bytes(&mut self, stream: &mut [u8], bytes: usize) {
            let zeroed_buffer = self.allocate_memory(bytes);
            self.encrypt_bytes(zeroed_buffer, stream, bytes);
        }


        fn string_to_key(&self, s: &str) -> [u8; 32] {
            let mut key: [u8; 32] = [0; 32];
            for i in 0 .. s.len() {
                let ptr: *const u8 = (s.as_ptr() as usize + i) as *const u8;
                key[i] = unsafe { *ptr };
            }
            return key;
        }

        pub fn key_setup(&mut self, k: &[u8], k_bits: usize, _iv_bits: usize) {
            let mut k_index: usize = 0;
            let k_slice = &k[k_index + (4 * 0) .. k_index + (4 * 1)];
            self.input[4] = u8_to_u32_little(&k_slice);
            let k_slice = &k[k_index + (4 * 1) .. k_index + (4 * 2)];
            self.input[5] = u8_to_u32_little(&k_slice);
            let k_slice = &k[k_index + (4 * 2) .. k_index + (4 * 3)];
            self.input[6] = u8_to_u32_little(&k_slice);
            let k_slice = &k[k_index + (4 * 3) .. k_index + (4 * 4)];
            self.input[7] = u8_to_u32_little(&k_slice);
            let constants: [u8; 32] = {
                if k_bits == 256 {
                    k_index += 16;
                    self.string_to_key(crate::chacha20::SIGMA)
                } else {
                    self.string_to_key(crate::chacha20::TAU)
                }
            };
            let k_slice = &k[k_index + (4 * 0) .. k_index + (4 * 1)];
            self.input[8] = u8_to_u32_little(k_slice);
            let k_slice = &k[k_index + (4 * 1) .. k_index + (4 * 2)];
            self.input[9] = u8_to_u32_little(k_slice);
            let k_slice = &k[k_index + (4 * 2) .. k_index + (4 * 3)];
            self.input[10] = u8_to_u32_little(k_slice);
            let k_slice = &k[k_index + (4 * 3) .. k_index + (4 * 4)];
            self.input[11] = u8_to_u32_little(&k_slice);
            self.input[0] = u8_to_u32_little(&constants[(4 * 0) .. (4 * 1)]);
            self.input[1] = u8_to_u32_little(&constants[(4 * 1) .. (4 * 2)]);
            self.input[2] = u8_to_u32_little(&constants[(4 * 2) .. (4 * 3)]);
            self.input[3] = u8_to_u32_little(&constants[(4 * 3) .. (4 * 4)]);
        }

        pub fn iv_setup(&mut self, iv: &[u8]) {
            let iv_slice: &[u8] = &iv[0 .. 12];
            self.input[13] = u8_to_u32_little(&iv_slice[(4 * 0) .. (4 * 1)]);
            self.input[14] = u8_to_u32_little(&iv_slice[(4 * 1) .. (4 * 2)]);
            self.input[15] = u8_to_u32_little(&iv_slice[(4 * 2) .. (4 * 3)]);
        }

        pub fn block_counter_setup(&mut self, block_counter: u32) {
            self.input[12] = block_counter;
        }

        fn allocate_memory(&mut self, bytes: usize) -> &'static mut [u8] {
            let buffer: *mut u8 = (self.malloc)(bytes);
            let buffer_slice: &mut [u8] = unsafe {
                core::slice::from_raw_parts_mut(buffer, bytes)
            };
            let mut iterator = buffer_slice.iter_mut();
            for c in iterator.next() {
                *c = 0;
            }
            return buffer_slice;
        }

        pub fn new(malloc: Allocator, free: Free) -> Self {
            return ChaCha20 {
                malloc: malloc,
                free: free,
                input: [0; 16],
            };
        }

        pub fn test_quarter_round() -> bool {
            assert_eq!(rotate_left(0xaabbccdd, 8), 0xbbccddaa);
            let input: [u32; 16] = [
                0x879531e0, 0xc5ecf37d, 0x516461b1, 0xc9a62f8a,
                0x44c20ef3, 0x3390af7f, 0xd9fc690b, 0x2a5f714c,
                0x53372767, 0xb00a5631, 0x974c541a, 0x359e9963,
                0x5c971061, 0x3d631689, 0x2098d9d6, 0x91dbd320,
            ];
            let output = quarter_round(input, 2, 7, 8, 13);
            let expected_matrix = [
                0x879531e0, 0xc5ecf37d, 0xbdb886dc, 0xc9a62f8a,
                0x44c20ef3, 0x3390af7f, 0xd9fc690b, 0xcfacafd2,
                0xe46bea80, 0xb00a5631, 0x974c541a, 0x359e9963,
                0x5c971061, 0xccc07c79, 0x2098d9d6, 0x91dbd320,
            ];
            if output != expected_matrix {
                assert_eq!(output, expected_matrix);
                return false;
            }
            return true;
        }

        pub fn check_state(&self, input: &[u32]) -> bool {
            let rc: bool = if self.input == input {
                true
            } else {
                false
            };
            return rc;
        }

        pub fn test1(malloc: Allocator, free: Free) {
            let key: [u8; 32] = [0; 32];
            let iv: [u8; 12] = [0; 12];
            let block_counter: u32 = 0;
            let mut m: [u8; 64] = [0; 64];
            let mut c: [u8; 64] = [0; 64];
            let mut obj = ChaCha20::new(malloc, free);
            obj.key_setup(&key[0 .. 32], 256, 0);
            obj.iv_setup(&iv[0 .. 12]);
            obj.block_counter_setup(block_counter);
            let current_state: [u32; 16] = obj.dump_state();
            assert_eq!(current_state, crate::chacha20::TEST1_EXPECTED_STATE);
            let len: usize = m.len();
            obj.encrypt_bytes(&mut m, &mut c, len);
            assert_eq!(c, *&crate::chacha20::TEST1_EXPECTED_CYPHERTEXT);
        }
    }

    pub fn key_setup(handle: *mut ChaCha20, k: *const u8, k_bits: usize, _iv_bits: usize) {
        assert_ne!(handle as usize, 0);
        assert_eq!(k_bits % 8, 0);
        let k_slice = unsafe { core::slice::from_raw_parts(k, k_bits / 8) };
        let obj: &mut ChaCha20 = unsafe { &mut *handle };
        obj.key_setup(k_slice, k_bits, _iv_bits);
    }

    pub fn iv_setup(handle: *mut ChaCha20, iv: *const u8) {
        assert_ne!(handle as usize, 0);
        assert_ne!(iv as usize, 0);
        let iv_slice = unsafe { core::slice::from_raw_parts(iv, 12) };
        let obj: &mut ChaCha20 = unsafe { &mut *handle };
        obj.iv_setup(iv_slice);
    }

    pub fn block_counter_setup(handle: *mut ChaCha20, block_counter: u32) {
        assert_ne!(handle as usize, 0);
        let obj: &mut ChaCha20 = unsafe { &mut *handle };
            obj.block_counter_setup(block_counter);
    }

    pub fn encrypt_bytes(handle: *mut ChaCha20, m: *const u8, c: *mut u8, bytes: usize) {
        let m_slice = unsafe {
            core::slice::from_raw_parts_mut(m as *mut u8, bytes)
        };
        let c_slice = unsafe {
            core::slice::from_raw_parts_mut(c, bytes)
        };
        let obj: &mut ChaCha20 = unsafe {
            &mut *handle
        };
        obj.encrypt_bytes(m_slice, c_slice, bytes);
    }

    pub fn keystream_bytes(handle: *mut ChaCha20, stream: *mut u8, bytes: usize) {
        let stream_slice = unsafe {
            core::slice::from_raw_parts_mut(stream, bytes)
        };
        let obj: &mut ChaCha20 = unsafe {
            &mut *handle
        };
        let mut iter =  stream_slice.iter_mut();
        for c in iter.next() {
            *c = 0;
        }
        obj.keystream_bytes(stream_slice, bytes);
    }

}

#[cfg(test)]
pub mod tests {

    use super::chacha20::*;

    #[test]
    fn quarter_round() {
        let rc = ChaCha20::test_quarter_round();
        assert_eq!(rc, true);
    }

    #[test]
    fn test1() {
        ChaCha20::test1(dummy_allocator, dummy_free);
    }

    extern "C"
    fn dummy_allocator(bytes: usize) -> *mut u8 {
        return 0 as *mut u8;
    }

    extern "C"
    fn dummy_free(ptr: *mut u8) {}

}
