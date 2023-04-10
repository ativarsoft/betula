// Copyright (C) 2023 Mateus de Lima Oliveira

#![no_std]

static SIGMA: &'static str = "expand 32-byte k";
static TAU: &'static str = "expand 16-byte k";

/*
 * Test vectors
 *
 * Test vectors can be found on Appendix 2 of RFC 7539.
 *
 * Test vectors are declared as global static constants
 * in order to suggest the compiler to place them on
 * the read-only section of the executable file.
 *
 * The ELF binary file format is used on Linux and
 * the PE binary file format is used on Microsoft Windows.
 * Those binary file formats support read-only sections.
 *
 * The read-only section is usually named ".rodata".
 */

static TEST1_KEY: [u8; 32] = [0; 32];

static TEST1_IV: [u8; 12] = [0; 12];

static TEST1_INITIAL_BLOCK_COUNTER: u32 = 0;

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

static TEST2_PLAINTEXT: [u8; 375] =
    [
        0x41, 0x6e, 0x79, 0x20, 0x73, 0x75, 0x62, 0x6d,
        0x69, 0x73, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x74,
        0x6f, 0x20, 0x74, 0x68, 0x65, 0x20, 0x49, 0x45,
        0x54, 0x46, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x6e,
        0x64, 0x65, 0x64, 0x20, 0x62, 0x79, 0x20, 0x74,
        0x68, 0x65, 0x20, 0x43, 0x6f, 0x6e, 0x74, 0x72,
        0x69, 0x62, 0x75, 0x74, 0x6f, 0x72, 0x20, 0x66,
        0x6f, 0x72, 0x20, 0x70, 0x75, 0x62, 0x6c, 0x69,
        0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x61,
        0x73, 0x20, 0x61, 0x6c, 0x6c, 0x20, 0x6f, 0x72,
        0x20, 0x70, 0x61, 0x72, 0x74, 0x20, 0x6f, 0x66,
        0x20, 0x61, 0x6e, 0x20, 0x49, 0x45, 0x54, 0x46,
        0x20, 0x49, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x65,
        0x74, 0x2d, 0x44, 0x72, 0x61, 0x66, 0x74, 0x20,
        0x6f, 0x72, 0x20, 0x52, 0x46, 0x43, 0x20, 0x61,
        0x6e, 0x64, 0x20, 0x61, 0x6e, 0x79, 0x20, 0x73,
        0x74, 0x61, 0x74, 0x65, 0x6d, 0x65, 0x6e, 0x74,
        0x20, 0x6d, 0x61, 0x64, 0x65, 0x20, 0x77, 0x69,
        0x74, 0x68, 0x69, 0x6e, 0x20, 0x74, 0x68, 0x65,
        0x20, 0x63, 0x6f, 0x6e, 0x74, 0x65, 0x78, 0x74,
        0x20, 0x6f, 0x66, 0x20, 0x61, 0x6e, 0x20, 0x49,
        0x45, 0x54, 0x46, 0x20, 0x61, 0x63, 0x74, 0x69,
        0x76, 0x69, 0x74, 0x79, 0x20, 0x69, 0x73, 0x20,
        0x63, 0x6f, 0x6e, 0x73, 0x69, 0x64, 0x65, 0x72,
        0x65, 0x64, 0x20, 0x61, 0x6e, 0x20, 0x22, 0x49,
        0x45, 0x54, 0x46, 0x20, 0x43, 0x6f, 0x6e, 0x74,
        0x72, 0x69, 0x62, 0x75, 0x74, 0x69, 0x6f, 0x6e,
        0x22, 0x2e, 0x20, 0x53, 0x75, 0x63, 0x68, 0x20,
        0x73, 0x74, 0x61, 0x74, 0x65, 0x6d, 0x65, 0x6e,
        0x74, 0x73, 0x20, 0x69, 0x6e, 0x63, 0x6c, 0x75,
        0x64, 0x65, 0x20, 0x6f, 0x72, 0x61, 0x6c, 0x20,
        0x73, 0x74, 0x61, 0x74, 0x65, 0x6d, 0x65, 0x6e,
        0x74, 0x73, 0x20, 0x69, 0x6e, 0x20, 0x49, 0x45,
        0x54, 0x46, 0x20, 0x73, 0x65, 0x73, 0x73, 0x69,
        0x6f, 0x6e, 0x73, 0x2c, 0x20, 0x61, 0x73, 0x20,
        0x77, 0x65, 0x6c, 0x6c, 0x20, 0x61, 0x73, 0x20,
        0x77, 0x72, 0x69, 0x74, 0x74, 0x65, 0x6e, 0x20,
        0x61, 0x6e, 0x64, 0x20, 0x65, 0x6c, 0x65, 0x63,
        0x74, 0x72, 0x6f, 0x6e, 0x69, 0x63, 0x20, 0x63,
        0x6f, 0x6d, 0x6d, 0x75, 0x6e, 0x69, 0x63, 0x61,
        0x74, 0x69, 0x6f, 0x6e, 0x73, 0x20, 0x6d, 0x61,
        0x64, 0x65, 0x20, 0x61, 0x74, 0x20, 0x61, 0x6e,
        0x79, 0x20, 0x74, 0x69, 0x6d, 0x65, 0x20, 0x6f,
        0x72, 0x20, 0x70, 0x6c, 0x61, 0x63, 0x65, 0x2c,
        0x20, 0x77, 0x68, 0x69, 0x63, 0x68, 0x20, 0x61,
        0x72, 0x65, 0x20, 0x61, 0x64, 0x64, 0x72, 0x65,
        0x73, 0x73, 0x65, 0x64, 0x20, 0x74, 0x6f
    ];

static TEST2_EXPECTED_CYPHERTEXT: [u8; 375] =
    [
        0xa3, 0xfb, 0xf0, 0x7d, 0xf3, 0xfa, 0x2f, 0xde,
        0x4f, 0x37, 0x6c, 0xa2, 0x3e, 0x82, 0x73, 0x70,
        0x41, 0x60, 0x5d, 0x9f, 0x4f, 0x4f, 0x57, 0xbd,
        0x8c, 0xff, 0x2c, 0x1d, 0x4b, 0x79, 0x55, 0xec,
        0x2a, 0x97, 0x94, 0x8b, 0xd3, 0x72, 0x29, 0x15,
        0xc8, 0xf3, 0xd3, 0x37, 0xf7, 0xd3, 0x70, 0x05,
        0x0e, 0x9e, 0x96, 0xd6, 0x47, 0xb7, 0xc3, 0x9f,
        0x56, 0xe0, 0x31, 0xca, 0x5e, 0xb6, 0x25, 0x0d,
        0x40, 0x42, 0xe0, 0x27, 0x85, 0xec, 0xec, 0xfa,
        0x4b, 0x4b, 0xb5, 0xe8, 0xea, 0xd0, 0x44, 0x0e,
        0x20, 0xb6, 0xe8, 0xdb, 0x09, 0xd8, 0x81, 0xa7,
        0xc6, 0x13, 0x2f, 0x42, 0x0e, 0x52, 0x79, 0x50,
        0x42, 0xbd, 0xfa, 0x77, 0x73, 0xd8, 0xa9, 0x05,
        0x14, 0x47, 0xb3, 0x29, 0x1c, 0xe1, 0x41, 0x1c,
        0x68, 0x04, 0x65, 0x55, 0x2a, 0xa6, 0xc4, 0x05,
        0xb7, 0x76, 0x4d, 0x5e, 0x87, 0xbe, 0xa8, 0x5a,
        0xd0, 0x0f, 0x84, 0x49, 0xed, 0x8f, 0x72, 0xd0,
        0xd6, 0x62, 0xab, 0x05, 0x26, 0x91, 0xca, 0x66,
        0x42, 0x4b, 0xc8, 0x6d, 0x2d, 0xf8, 0x0e, 0xa4,
        0x1f, 0x43, 0xab, 0xf9, 0x37, 0xd3, 0x25, 0x9d,
        0xc4, 0xb2, 0xd0, 0xdf, 0xb4, 0x8a, 0x6c, 0x91,
        0x39, 0xdd, 0xd7, 0xf7, 0x69, 0x66, 0xe9, 0x28,
        0xe6, 0x35, 0x55, 0x3b, 0xa7, 0x6c, 0x5c, 0x87,
        0x9d, 0x7b, 0x35, 0xd4, 0x9e, 0xb2, 0xe6, 0x2b,
        0x08, 0x71, 0xcd, 0xac, 0x63, 0x89, 0x39, 0xe2,
        0x5e, 0x8a, 0x1e, 0x0e, 0xf9, 0xd5, 0x28, 0x0f,
        0xa8, 0xca, 0x32, 0x8b, 0x35, 0x1c, 0x3c, 0x76,
        0x59, 0x89, 0xcb, 0xcf, 0x3d, 0xaa, 0x8b, 0x6c,
        0xcc, 0x3a, 0xaf, 0x9f, 0x39, 0x79, 0xc9, 0x2b,
        0x37, 0x20, 0xfc, 0x88, 0xdc, 0x95, 0xed, 0x84,
        0xa1, 0xbe, 0x05, 0x9c, 0x64, 0x99, 0xb9, 0xfd,
        0xa2, 0x36, 0xe7, 0xe8, 0x18, 0xb0, 0x4b, 0x0b,
        0xc3, 0x9c, 0x1e, 0x87, 0x6b, 0x19, 0x3b, 0xfe,
        0x55, 0x69, 0x75, 0x3f, 0x88, 0x12, 0x8c, 0xc0,
        0x8a, 0xaa, 0x9b, 0x63, 0xd1, 0xa1, 0x6f, 0x80,
        0xef, 0x25, 0x54, 0xd7, 0x18, 0x9c, 0x41, 0x1f,
        0x58, 0x69, 0xca, 0x52, 0xc5, 0xb8, 0x3f, 0xa3,
        0x6f, 0xf2, 0x16, 0xb9, 0xc1, 0xd3, 0x00, 0x62,
        0xbe, 0xbc, 0xfd, 0x2d, 0xc5, 0xbc, 0xe0, 0x91,
        0x19, 0x34, 0xfd, 0xa7, 0x9a, 0x86, 0xf6, 0xe6,
        0x98, 0xce, 0xd7, 0x59, 0xc3, 0xff, 0x9b, 0x64,
        0x77, 0x33, 0x8f, 0x3d, 0xa4, 0xf9, 0xcd, 0x85,
        0x14, 0xea, 0x99, 0x82, 0xcc, 0xaf, 0xb3, 0x41,
        0xb2, 0x38, 0x4d, 0xd9, 0x02, 0xf3, 0xd1, 0xab,
        0x7a, 0xc6, 0x1d, 0xd2, 0x9c, 0x6f, 0x21, 0xba,
        0x5b, 0x86, 0x2f, 0x37, 0x30, 0xe3, 0x7c, 0xfd,
        0xc4, 0xfd, 0x80, 0x6c, 0x22, 0xf2, 0x21
    ];

      static TEST3_KEY: [u8; 32] =
        [0x1c, 0x92, 0x40, 0xa5, 0xeb, 0x55, 0xd3, 0x8a,
         0xf3, 0x33, 0x88, 0x86, 0x04, 0xf6, 0xb5, 0xf0,
         0x47, 0x39, 0x17, 0xc1, 0x40, 0x2b, 0x80, 0x09,
         0x9d, 0xca, 0x5c, 0xbc, 0x20, 0x70, 0x75, 0xc0];

      static TEST3_IV: [u8; 12] =
        [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x02];

      static TEST3_INITIAL_BLOCK_COUNTER : u32 = 42;

      static TEST3_PLAINTEXT: [u8; 127] =
        [0x27, 0x54, 0x77, 0x61, 0x73, 0x20, 0x62, 0x72,
         0x69, 0x6c, 0x6c, 0x69, 0x67, 0x2c, 0x20, 0x61,
         0x6e, 0x64, 0x20, 0x74, 0x68, 0x65, 0x20, 0x73,
         0x6c, 0x69, 0x74, 0x68, 0x79, 0x20, 0x74, 0x6f,
         0x76, 0x65, 0x73, 0x0a, 0x44, 0x69, 0x64, 0x20,
         0x67, 0x79, 0x72, 0x65, 0x20, 0x61, 0x6e, 0x64,
         0x20, 0x67, 0x69, 0x6d, 0x62, 0x6c, 0x65, 0x20,
         0x69, 0x6e, 0x20, 0x74, 0x68, 0x65, 0x20, 0x77,
         0x61, 0x62, 0x65, 0x3a, 0x0a, 0x41, 0x6c, 0x6c,
         0x20, 0x6d, 0x69, 0x6d, 0x73, 0x79, 0x20, 0x77,
         0x65, 0x72, 0x65, 0x20, 0x74, 0x68, 0x65, 0x20,
         0x62, 0x6f, 0x72, 0x6f, 0x67, 0x6f, 0x76, 0x65,
         0x73, 0x2c, 0x0a, 0x41, 0x6e, 0x64, 0x20, 0x74,
         0x68, 0x65, 0x20, 0x6d, 0x6f, 0x6d, 0x65, 0x20,
         0x72, 0x61, 0x74, 0x68, 0x73, 0x20, 0x6f, 0x75,
         0x74, 0x67, 0x72, 0x61, 0x62, 0x65, 0x2e];

      static TEST3_EXPECTED_CYPHERTEXT: [u8; 127] =
        [0x62, 0xe6, 0x34, 0x7f, 0x95, 0xed, 0x87, 0xa4,
         0x5f, 0xfa, 0xe7, 0x42, 0x6f, 0x27, 0xa1, 0xdf,
         0x5f, 0xb6, 0x91, 0x10, 0x04, 0x4c, 0x0d, 0x73,
         0x11, 0x8e, 0xff, 0xa9, 0x5b, 0x01, 0xe5, 0xcf,
         0x16, 0x6d, 0x3d, 0xf2, 0xd7, 0x21, 0xca, 0xf9,
         0xb2, 0x1e, 0x5f, 0xb1, 0x4c, 0x61, 0x68, 0x71,
         0xfd, 0x84, 0xc5, 0x4f, 0x9d, 0x65, 0xb2, 0x83,
         0x19, 0x6c, 0x7f, 0xe4, 0xf6, 0x05, 0x53, 0xeb,
         0xf3, 0x9c, 0x64, 0x02, 0xc4, 0x22, 0x34, 0xe3,
         0x2a, 0x35, 0x6b, 0x3e, 0x76, 0x43, 0x12, 0xa6,
         0x1a, 0x55, 0x32, 0x05, 0x57, 0x16, 0xea, 0xd6,
         0x96, 0x25, 0x68, 0xf8, 0x7d, 0x3f, 0x3f, 0x77,
         0x04, 0xc6, 0xa8, 0xd1, 0xbc, 0xd1, 0xbf, 0x4d,
         0x50, 0xd6, 0x15, 0x4b, 0x6d, 0xa7, 0x31, 0xb1,
         0x87, 0xb5, 0x8d, 0xfd, 0x72, 0x8a, 0xfa, 0x36,
         0x75, 0x7a, 0x79, 0x7a, 0xc1, 0x88, 0xd1];


pub mod chacha20 {

    pub type Allocator = unsafe extern "C" fn(bytes: usize) -> *mut u8;
    pub type Free = unsafe extern "C" fn(*mut u8);

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

        pub fn encrypt_bytes(&mut self, m: &[u8], c: &mut [u8], bytes: usize) {
            assert_ne!(bytes, 0);
            assert_eq!(m.len() % 4, 0);
            assert_eq!(c.len() % 4, 0);
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

        fn allocate_memory(&self, bytes: usize) -> &'static mut [u8] {
            let buffer: *mut u8 = unsafe { (self.malloc)(bytes) };
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

        pub fn pad<'a>(&self, buffer: &'a [u8]) -> &'a [u8] {
            return if buffer.len() % 4 == 0 {
                buffer
            } else {
                let len: usize = (buffer.len() >> 2) << 2  + 4;

                let output: &mut [u8] = self.allocate_memory(len);
                let mut output_iterator = output.iter_mut();
                for input_character in buffer {
                    let output_character = match output_iterator.next() {
                        Some(c) => c,
                        None => panic!(),
                    };
                    *output_character = *input_character;
                };
                output
            }
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

        /*
         * Tests
         */

        /*
         * These tests use the test vectors on the top of this file.
         */

        pub fn test1(malloc: Allocator, free: Free) {
            let key: &[u8] = &crate::chacha20::TEST1_KEY;
            let iv: &[u8] = &crate::chacha20::TEST1_IV;
            let block_counter: u32 = crate::chacha20::TEST1_INITIAL_BLOCK_COUNTER;
            let mut m: [u8; 64] = [0; 64];
            let mut c: [u8; 64] = [0; 64];
            let mut obj = ChaCha20::new(malloc, free);
            obj.key_setup(key, 256, 0);
            obj.iv_setup(&iv[0 .. 12]);
            obj.block_counter_setup(block_counter);
            let len: usize = m.len();
            obj.encrypt_bytes(&mut m, &mut c, len);
            let current_state: [u32; 16] = obj.dump_state();
            assert_eq!(c, *&crate::chacha20::TEST1_EXPECTED_CYPHERTEXT);
        }

        pub fn test2(malloc: Allocator, free: Free) {
            let mut key: [u8; 32] = [0; 32];
            key[31] = 1;
            let mut iv: [u8; 12] = [0; 12];
            iv[11] = 2;
            let block_counter: u32 = 1;
            let mut obj = ChaCha20::new(malloc, free);
            let plaintext: &[u8] = &crate::chacha20::TEST2_PLAINTEXT;
            let m: &[u8] = obj.pad(plaintext);
            let mut c: &mut [u8] = obj.allocate_memory(m.len());
            obj.key_setup(&key[0 .. 32], 256, 0);
            obj.iv_setup(&iv[0 .. 12]);
            obj.block_counter_setup(block_counter);
            let len: usize = plaintext.len();
            assert_eq!(len, 375);
            obj.encrypt_bytes(m, c, len);
            let expected_cyphertext = crate::chacha20::TEST2_EXPECTED_CYPHERTEXT;
            assert_eq!(c[0 .. plaintext.len()], expected_cyphertext);
        }

        pub fn test3(malloc: Allocator, free: Free) {
            let mut key: &[u8] = &crate::chacha20::TEST3_KEY;
            let mut iv: &[u8] = &crate::chacha20::TEST3_IV;
            let block_counter: u32 = crate::chacha20::TEST3_INITIAL_BLOCK_COUNTER;
            let mut obj = ChaCha20::new(malloc, free);
            let plaintext: &[u8] = &crate::chacha20::TEST3_PLAINTEXT;
            let m: &[u8] = obj.pad(plaintext);
            let mut c: &mut [u8] = obj.allocate_memory(m.len());
            obj.key_setup(&key[0 .. 32], 256, 0);
            obj.iv_setup(&iv[0 .. 12]);
            obj.block_counter_setup(block_counter);
            let len: usize = plaintext.len();
            assert_eq!(len, 127);
            obj.encrypt_bytes(m, c, len);
            let expected_cyphertext = crate::chacha20::TEST3_EXPECTED_CYPHERTEXT;
            assert_eq!(c[0 .. plaintext.len()], expected_cyphertext);
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

    extern "C" {
        fn malloc(bytes: usize) -> *mut u8;
    }

    #[test]
    fn quarter_round() {
        let rc = ChaCha20::test_quarter_round();
        assert_eq!(rc, true);
    }

    #[test]
    fn test1() {
        ChaCha20::test1(dummy_allocator, dummy_free);
    }

    #[test]
    fn test2() {
        ChaCha20::test2(malloc, dummy_free);
    }

    #[test]
    fn test3() {
        ChaCha20::test3(malloc, dummy_free);
    }

    extern "C"
    fn dummy_allocator(_bytes: usize) -> *mut u8 {
        return 0 as *mut u8;
    }

    extern "C"
    fn dummy_free(_ptr: *mut u8) {}

}

