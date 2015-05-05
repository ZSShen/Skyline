#!/usr/bin/python


import os;
import sys;
import argparse;
import shutil;


def main():

    parser = argparse.ArgumentParser();
    parser.add_argument("--rebuild", dest = "rebuild", action = "store_true");
    parser.add_argument("--no-rebuild", dest = "rebuild", action = "store_false");
    parser.set_defaults(rebuild = False);

    args = parser.parse_args();
    rebuild = args.rebuild;

    # Recursivly clean all the binary directories.
    list_clean = list();
    path_cur = os.getcwd();
    path_build = os.path.join(path_cur, "build");
    if os.path.isdir(path_build) == True:
        shutil.rmtree(path_build);
    path_bin_plg = os.path.join(path_cur, "bin", "plugin");
    if os.path.isdir(path_bin_plg) == True:
        shutil.rmtree(path_bin_plg);
    path_bin_eng = os.path.join(path_cur, "bin", "engine");
    if os.path.isdir(path_bin_eng) == True:
        shutil.rmtree(path_bin_eng);

    # Create the build folder if necessary.
    if rebuild == True:
        if os.path.isdir(path_build) == False:
            os.makedirs(path_build);

    return;


if __name__ == "__main__":
    main();
