// Copyright (C) 2018 Mateus de Lima Oliveira

extern crate xml;

use std::fs::File;
use std::io::BufReader;
//use std::path::Path;

use xml::reader::{EventReader, XmlEvent};
use xml::name::{OwnedName};
use xml::attribute::{OwnedAttribute};

use std::env;
use std::collections::LinkedList;

enum Node {
    Start {
        // copy name and attributes from parsed values
        name: OwnedName,
        attributes: Vec<OwnedAttribute>,

        jmp: Option<usize>, // Offset to end node.
    },
    End {
        // copy name from parsed values
        name: OwnedName,

        conditional_jmp: bool,
        jmp: Option<usize>, // Offset to start node
    },
    CharacterData {
        // copy data from parsed values
        text: String,
    }
}

/*enum Jump {
    JmpBackwards,
    JmpForward,
}*/

pub enum ControlFlow {
    JumpInstruction,
    NextInstruction
}

/* Sequential user script input with type checking (text or control) */
enum Input {
    FillerText {
        text: String
    },
    ControlFlow {
        control: ControlFlow
    }
}

pub struct Context<'a> {
    /* This list contains template nodes. */
    nodes: Vec<Node>,
    /* This list is freed as it is consumed because it does not loop. */
    /* It contains data from user input */
    input: LinkedList<Input>,
    /* These stacks contain control flow information from template. */
    //tags: Vec<String>,
    //labels: Vec<StartNodeIndex>,
    tmpl: &'a String
}

fn is_control_flow_keyword(el: &String) -> bool {
    return if el == "if" || el == "swhile" || el == "ewhile" {
        true
    } else {
        false
    }
}

fn print_start_node(name: &OwnedName, attributes: &Vec<OwnedAttribute>, input: &mut LinkedList<Input>) {
    if name.local_name == "templatizer" {
        return;
    }
    if is_control_flow_keyword(&name.local_name) {
        return;
    }
    print!("<{}", name.local_name);
    for a in attributes {
        if a.value == "@" {
            /* dump string from filler text list from first to last */
            match input.pop_back() {
                Some(x) => {
                    /* Input type checking. */
                    match x {
                        Input::FillerText {text} => {
                            print!(" {}=\"{}\"", a.name, text);
                        },
                        Input::ControlFlow {..} => {
                            panic!("Expecting text placeholer, found control flow.");
                        }
                    }
                },
                None => {
                    panic!("Unexpected text placeholder. List is empty.");
                }
            };
        } else {
            print!(" {}=\"{}\"", a.name, a.value);
        }
    }
    print!(">");
}

fn print_end_node(name: &OwnedName) {
    if name.local_name == "templatizer" {
        return;
    }
    if is_control_flow_keyword(&name.local_name) {
        return;
    }
    print!("</{}>", name.local_name);
}

fn print_character_data_node(text: &String, input: &mut LinkedList<Input>) {
    for c in text.chars() {
        if c == '@' {
            match input.pop_back() {
                Some(x) => {
                    /* Input type checking. */
                    match x {
                        Input::FillerText {text} => {
                            print!("{}", text);
                        },
                        Input::ControlFlow {..} => {
                            panic!("Expecting text placeholer, found control flow.");
                        }
                    }
                },
                None => {
                    panic!("Unexpected text placeholder. List is empty.");
                }
            };
        } else {
            print!("{}", c);
        }
    }
}

/*fn parse_template_tag(_data: &mut Context, _attributes: &Vec<OwnedAttribute>) {
    //This tag has not been obsoleted.
}*/

/*fn parse_include_tag(data: &mut Context, attributes: &Vec<OwnedAttribute>) {
    for attr in attributes {
        match &*attr.name.local_name {
            "file" => {
                let path = Path::new(&data.tmpl).parent().unwrap().join(&attr.value);
            },
            _ => (),
        }
    }
}*/

/*fn tag_pool_add(data: &mut Context, el: &String) {
}

fn tag_pool_lookup(data: &mut Context, el: &String) -> usize {
    for (idx, tag) in data.tags.iter().enumerate() {
        if tag == el {
            return idx;
        }
    }
    tag_pool_add(data, el);
    return data.tags.len();
}*/

/*fn start(data: &mut Context, name: &OwnedName, attributes: &Vec<OwnedAttribute>) {
    if name.local_name == "templatizer" {
        parse_template_tag(data, attributes);
    }
    if name.local_name == "include" {
        parse_include_tag(data, attributes);
    }
    tag_pool_lookup(data, &name.local_name);
}*/

/*fn read_input_jump_instruction(data: &mut Context) -> ControlFlow {
    match data.input.pop_back() {
        Some(Input::ControlFlow {control}) => {
            control
        },
        Some(Input::FillerText {..}) => {
            panic!("Expecting control flow input. Found filler text.");
        }
        None => {
            panic!("missing input for tag that requires control flow input");
        }
    }
}*/

/* print node */
fn interpret_template_node(data: &mut Context, i: usize) -> Option<usize> {
    match data.nodes[i] {
        Node::Start {ref name, ref attributes, jmp} => {
            print_start_node(name, attributes, &mut data.input);
            jmp
        },
        Node::End {ref name, conditional_jmp, jmp} => {
            print_end_node(name);
            if conditional_jmp {
                jmp
            } else {
                None
            }
        },
        Node::CharacterData {ref text} => {
            print_character_data_node(text, &mut data.input);
            None
        },
    }
}

/* print list */
pub fn print_xml_file(data: &mut Context) {
    let len = data.nodes.len();
    if len == 0 {
        panic!("no nodes to print");
    }
    let mut i = 0;
    while i < len {
        match interpret_template_node(data, i) {
            Some(idx) => {
                i = idx;
            },
            None => {
                i = i + 1;
            }
        }
    }
	
    if data.input.is_empty() == false {
        panic!("Trailing data at end of the input list.");
    }
}

pub fn parse_xml_file<'a>(data: &mut Context) {
    let file = match File::open(&*data.tmpl) {
        Ok(x) => x,
        Err(_) => {
            eprintln!("Unable to open template file.");
            return
        },
    };
    let file = BufReader::new(file);

    let parser = EventReader::new(file);
    for e in parser {
        match e {
            Ok(XmlEvent::StartElement { name, attributes, .. }) => {
                data.nodes.push(Node::Start {
                    name: name.clone(),
                    attributes: attributes.clone(),
                    jmp: None
                });
            }
            Ok(XmlEvent::EndElement { name }) => {
                data.nodes.push(Node::End {
                    name: name.clone(),
                    conditional_jmp: false,
                    jmp: None
                });
            }
            Ok (XmlEvent::Characters(text)) => {
                data.nodes.push(Node::CharacterData {
                    text: text.clone()
                });
            }
            Err(e) => {
                println!("Error: {}", e);
                break;
            }
            _ => {}
        }
    }
}

pub fn new<'a>(tmpl: &'a String) -> Context<'a> {
    Context {
        nodes: Vec::new(),
        input: LinkedList::new(),
        //tags: Vec::new(),
        //labels: Vec::new(),
        tmpl: tmpl
    }
}

pub fn default_tmpl() -> String {
    match env::var("PATH_TRANSLATED") {
        Ok(x) => x,
        Err(_e) => panic!("PATH_TRANSLATED env not found."),
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn index() {
        /*println!("Hello, world!");*/
        //let tmpl = default_tmpl();
        let tmpl = "index.tmpl".to_string();
        let mut data = new(&tmpl);
        add_filler_text(&mut data, "value");
        add_filler_text(&mut data, "Hello world.");
        parse_xml_file(&mut data);
        print_xml_file(&mut data);

        assert_eq!(2 + 2, 4);
    }
}

pub fn add_filler_text(data: &mut Context, text: &str)
{
    data.input.push_front(Input::FillerText {
        text: text.to_string()
    });
}

pub fn add_control_flow(data: &mut Context, cf: ControlFlow)
{
    data.input.push_front(Input::ControlFlow {
        control: cf
    });
}

