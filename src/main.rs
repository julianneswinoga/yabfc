#[macro_use] extern crate clap;
use clap::{Arg, App};

#[derive(Debug)]
struct GLobalOptions {
    quiet: bool,
    verbose: u8,
    optimize: u8,
    input_files: Vec<String>,
    output_file: String,
}

fn parse_cli_opts() -> Result<GLobalOptions, String> {
    let matches = App::new("yabfc")
        .version("2.0")
        .author("Cameron Swinoga <cameronswinoga@gmail.com>")
        .about("Compiles one or multiple brainfuck files to the ELF file format, in 64 bit mode")
        .arg(Arg::with_name("quiet")
            .short("q")
            .long("quiet")
            .help("Turns off all output"))
        .arg(Arg::with_name("verbose")
            .short("v")
            .multiple(true)
            .help("Sets the level of verbosity"))
        .arg(Arg::with_name("optimize")
            .value_name("optimize")
            .short("O")
            .long("optimize")
            .takes_value(true)
            .help("Sets the level of optimization"))
        .arg(Arg::with_name("output_file")
            .value_name("output_file")
            .short("o")
            .long("output_file")
            .takes_value(true)
            .help("Output to file name"))
        .arg(Arg::with_name("Files")
            .help("Input brainfuck files to compile")
            .required(true)
            .multiple(true))
        .get_matches();

    let quiet = matches.is_present("quiet");
    let verbose = matches.occurrences_of("verbose") as u8;
    let optimize = matches.value_of("optimize")
        .unwrap_or("3").parse::<u8>().unwrap();
    let input_files = matches.values_of("Files")
        .unwrap().map(|f| f.to_string()).collect::<Vec<String>>();
    let output_file = matches.value_of("output_file")
        .unwrap_or("").to_string();
    
    let output_file = match (input_files.len(), output_file.is_empty()) {
        (1, _) => output_file,
        (_, true) => output_file,
        (_, false) => {
            return Err("Cannot have output file with multiple input files present!".to_string())
        }
    };

    Ok(GLobalOptions {
        quiet: quiet,
        verbose: verbose,
        optimize: optimize,
        input_files: input_files,
        output_file: output_file,
    })
}

fn main() {
    let options = match parse_cli_opts() {
        Ok(o) => o,
        Err(e) => { println!("{}", e); return; }
    };

    println!("{:?}", options);
}
