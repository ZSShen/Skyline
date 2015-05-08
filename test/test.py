#!/usr/bin/python

import os;
import tarfile;
import sys;
import shutil;
import subprocess;


#-------- Constants for coverage testing --------
KEY_PATH_INPUT  = "-i"
KEY_PATH_OUTPUT = "-o"
KEY_DIMENSION   = "-d"
KEY_REPORT_TYPE = "-t"

VALUE_DIMENSION   = '2';
VALUE_REPORT_TYPE = "eti";

VALGRIND_PROG = "valgrind";
VALGRIND_ON_LEAK_CHECK = "--leak-check=yes";
VALGRIND_ON_TRACK_ORIGINS = "--track-origins=yes";


def main():

    path_cur_dir = os.getcwd();

    # The first argument stands for the path of executable.
    path_exec = sys.argv[1];

    # The second argument stands for the path of tar ball.
    path_case_tar = sys.argv[2];

    # The third argument stands for the path of temporary output folder.
    path_case = sys.argv[3];
    
    # Decompress the test cases.
    tar_ball = tarfile.open(path_case_tar);
    tar_ball.extractall(path_case);    
    
    # Iterative testing for each case.
    for path_dir, list_name_dir, list_name_file in os.walk(path_case):
        for name_file in list_name_file:
            # Generate the path string for input data.
            path_input = os.path.join(path_dir, name_file);

            # Generate the path string for output folder (ignoring .exe extension).
            path_output = path_input[:-4];

            # Generate complete command.
            command = list();
            command.append(VALGRIND_PROG);
            command.append(VALGRIND_ON_LEAK_CHECK);
            command.append(VALGRIND_ON_TRACK_ORIGINS);
            command.append(path_exec);
            command.append(KEY_PATH_INPUT);
            command.append(path_input);
            command.append(KEY_PATH_OUTPUT);
            command.append(path_output);
            command.append(KEY_DIMENSION);
            command.append(VALUE_DIMENSION);
            command.append(KEY_REPORT_TYPE);
            command.append(VALUE_REPORT_TYPE);

            # Execute the command.
            print command;
            proc = subprocess.Popen(command, stdout = subprocess.PIPE, shell = False);            
            while True:
                result = proc.stdout.read();
                if result == '':
                    break;
                print result;
            proc.wait();

    # Clean the folder.
    shutil.rmtree(path_case);

    return;


if __name__ == "__main__":
    main();
