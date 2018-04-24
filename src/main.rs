#[macro_use] extern crate log;
extern crate fern;
extern crate clap;

use clap::{Arg, App};
use std::fs::File;
use std::io::prelude::*;

static LEGAL_CHARS: &str = "+-><[],.";

#[derive(Debug)]
struct GLobalOptions {
    silent: bool,
    verbose: u8,
    optimize: u8,
    input_files: Vec<String>,
    output_file: String,
}

fn setup_logger(silent: bool, verbose_level: u8) -> Result<(), fern::InitError> {
    fern::Dispatch::new()
        .level(match (silent, verbose_level) {
            (true, _) => log::LevelFilter::Off,
            (_, 0) => log::LevelFilter::Warn,
            (_, 1) => log::LevelFilter::Info,
            (_, _) => log::LevelFilter::Debug, // Anything >= 2 is debug level
        })
        .chain(std::io::stdout())
        .apply()?;
    Ok(())
}

fn parse_cli_opts() -> Result<GLobalOptions, String> {
    let matches = App::new("yabfc")
        .version("2.0")
        .author("Cameron Swinoga <cameronswinoga@gmail.com>")
        .about("Compiles one or multiple brainfuck files to the ELF file format, in 64 bit mode")
        .arg(Arg::with_name("silent")
            .short("s")
            .long("silent")
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

    let silent = matches.is_present("silent");
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
        silent: silent,
        verbose: verbose,
        optimize: optimize,
        input_files: input_files,
        output_file: output_file,
    })
}

fn parse_files(input_files: Vec<String>) -> Result<(), std::io::Error> {
    info!("Starting file parse!");

    for filename in input_files {
        info!("Reading file {}", &filename);
        let mut file_contents = String::new();
        let mut f = File::open(filename)?;
        f.read_to_string(&mut file_contents)?;

        let legal_contents = file_contents.chars()
            .filter(|c: &char|
                LEGAL_CHARS.chars()
                    .any(|b: char| b == *c)
            ).collect::<String>();

        debug!("Read in file contents {}", &legal_contents);

        for c in legal_contents.chars() {
            // println!("{}", c);
        }
    }
    Ok(())
}

fn main() {
    let config = match parse_cli_opts() {
        Ok(o) => o,
        Err(e) => { println!("{}", e); return; }
    };

    match setup_logger(config.silent, config.verbose) {
        Ok(_) => (),
        Err(_) => { return; }
    };

    debug!("Silent: {}", config.silent);
    debug!("Verbose: {}", config.verbose);
    debug!("Optimize: {}", config.optimize);
    debug!("Input files: {:?}", config.input_files);
    debug!("Output file: {}", config.output_file);

    parse_files(config.input_files).expect("File parse error!");
}
