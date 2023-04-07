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

    fn u8_to_u32_little(data: [u8;4]) -> u32 {
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
        pub fn encrypt_bytes(&mut self, m: [u8; 64], mut c: [u8; 64], bytes: usize) {
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
    }
}

