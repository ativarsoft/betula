fn is_digit(d: char) -> bool {
    if d >= '0' && d <= '9' {
        return true;
    }
    return false;
}

pub fn get_number(s: &str) -> Option<i32> {
    let negative: bool = match s.chars().nth(0) {
        Some(c) => if c == '-'{
            true
        } else {
            false
        },
        None => return None,
    };
    let offset = if negative { 1 } else { 0 };
    let slice = &s[offset..];
    let mut i: usize = 0;
    for c in slice.chars() {
        if !is_digit(c) {
            break;
        }
        i = i + 1;
    }
    if i == 0 {
        return None;
    }
    let slice = &slice[..i];
    let mut slice = slice.chars();
    let mut n = 0;
    let mut multiplier = 1;
    loop {
        let digit: i32 = match slice.nth(i) {
            Some(d) => d as i32,
            None => break,
        };
        n = digit * multiplier;
        multiplier = multiplier * 10;
        i = i - 1;
        if i <= 0 {
            break;
        }
    }
    return Some(n);
}

#[derive(Copy, Clone)]
pub enum ProcessorIntrinsicOperations {
    ADD,
    SUB,
    MUL,
    DIV,
    MOD
}

type InstrinsicDispatchEntry = (&'static str, ProcessorIntrinsicOperations);

static INTRINSIC_DISPATCH: [InstrinsicDispatchEntry; 1] = [
    ("add", ProcessorIntrinsicOperations::ADD),
];

pub fn calc(op: ProcessorIntrinsicOperations, a: i32, b: i32) -> Option<i32> {
    let result: i32 = match op {
        ProcessorIntrinsicOperations::ADD => a + b,
        ProcessorIntrinsicOperations::SUB => a - b,
        ProcessorIntrinsicOperations::MUL => a * b,
        ProcessorIntrinsicOperations::DIV => {
            if b < 0 {
                return None;
            }
            a / b
        }
        ProcessorIntrinsicOperations::MOD => {
            if a < 0 || b < 0 {
                return None;
            }
            a % b
        }
    };
    return Some(result);
}

pub fn eval() {
    let op = "add";
    let a = 1;
    let b = 1;
    for entry in INTRINSIC_DISPATCH.iter() {
        if entry.0 == op {
            calc(entry.1, a, b);
        }
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
