static SIGMA: &'static str = "expand 32-byte k";
static TAU: &'static str = "expand 16-byte k";

pub mod chacha20 {

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

    fn quarter_round(x: [u32;16], a: u32, b: u32, c: u32, d: u32) -> [u32;16] {
        let mut x: [u32;16] = x;
        x[a as usize] = x[a as usize] + x[b as usize];
        x[d as usize] = rotate_left(x[d as usize] ^ x[a as usize], 16);
        x[c as usize] = x[c as usize] + x[d as usize];
        x[b as usize] = rotate_left(x[b as usize] ^ x[c as usize], 12);
        x[a as usize] = x[a as usize] + x[b as usize];
        x[d as usize] = rotate_left(x[d as usize] ^ x[a as usize], 8);
        x[c as usize] = x[c as usize] + x[d as usize];
        x[b as usize] = rotate_left(x[b as usize] ^ x[c as usize], 7);
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
            x[i as usize] = x[i as usize] + input[i as usize];
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
        input: [u32; 16],
    }

    impl ChaCha20 {
        pub fn encrypt_bytes(&mut self, m: *const u8, c: *mut u8, bytes: usize) {
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
                    for j in 0 .. (remaining_bytes - 1) {
                        let index = j + m_index;
                        let m_ptr = (m as usize + index) as *const u8;
                        let m_byte = unsafe { *m_ptr };
                        let c_byte = m_byte ^ output[j];
                        let index = j + c_index;
                        let c_ptr = (c as usize + index) as *mut u8;
                        unsafe {
                            *c_ptr = c_byte;
                        };
                    }
                    return;
                }
                for j in 0 .. 64 {
                    let index = j + m_index;
                    let m_ptr = (m as usize + index) as *const u8;
                    let m_byte = unsafe { *m_ptr };
                    let c_byte = m_byte ^ output[j];
                    let index = j + c_index;
                    let c_ptr = (c as usize + index) as *mut u8;
                    unsafe {
                        *c_ptr = c_byte;
                    };
                }
                remaining_bytes -= 64;
                c_index += 64;
                m_index += 64;
            }
        }

        pub fn decrypt_bytes(&mut self, m: *mut u8, c: *const u8, bytes: usize) {
            self.encrypt_bytes(c, m, bytes);
        }

        pub fn keystream_bytes(&mut self, stream: *mut u8, bytes: usize) {
            for i in 0 .. bytes {
                let stream_ptr = ((stream as usize) + i) as *mut u8;
                unsafe {
                    *stream_ptr = 0;
                };
            }
            self.encrypt_bytes(stream, stream, bytes);
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
            self.input[0] = u8_to_u32_little(&constants[0 .. (4 * 1)]);
            self.input[1] = u8_to_u32_little(&constants[0 .. (4 * 2)]);
            self.input[2] = u8_to_u32_little(&constants[0 .. (4 * 3)]);
            self.input[3] = u8_to_u32_little(&constants[0 .. (4 * 4)]);
        }

        pub fn test_quarter_round() -> bool {
            let input: [u32; 16] = [0; 16];
            quarter_round(input, 2, 7, 8, 13);
            // TODO: check output.
            return true;
        }
    }

    pub fn key_setup(handle: *mut ChaCha20, k: *const u8, k_bits: usize, _iv_bits: usize) {
        assert_ne!(handle as usize, 0);
        assert_eq!(k_bits % 8, 0);
        unsafe {
            let k_slice = core::slice::from_raw_parts(k, k_bits / 8);
            let mut obj: &mut ChaCha20 = &mut *handle;
            obj.key_setup(k_slice, k_bits, _iv_bits);
        };
    }
}

