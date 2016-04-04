#! /usr/bin/env python3

import os.path
import fnmatch
import re
import argparse
import tarfile

parser = argparse.ArgumentParser(description='DSnP bundle tool.')
parser.add_argument('student_id', nargs='?', default='b0xxxxxxx',
                    help='your student_id.')
parser.add_argument('directory', help='homework directory.')

FILENAME_MUST_EXIST  = "MustExist.txt"
FILENAME_MUST_REMOVE = "MustRemove.txt"
FILENAME_SELFCHECK   = "selfcheck.pl"

def check_remove(mrlist, name):
    for exclude in mrlist:
        if fnmatch.fnmatch(name, exclude):
            return True
    return False

def bundle(stu_id, hmdir):
    hmdir = os.path.normpath(hmdir)
    if not os.path.exists(hmdir):
        print("\033[91m\033[01mThe directory {} doesn't exist.\033[0m".format(hmdir))
        return 1

    list_file = os.path.join(hmdir, FILENAME_MUST_EXIST)
    if not os.path.isfile(list_file):
        print("\033[91m\033[01m{} doesn't exist in {}.\033[0m".format(FILENAME_MUST_EXIST, hmdir))
        return 1

    file_list = []
    outprefix = '{}_{}'.format(stu_id, hmdir)
    print("Output to \033[01m{}\033[0m...".format(outprefix))

    print("")

    with open(list_file, "r") as f:
        print("\033[94mIncluding: \033[0m")
        for line in f:
            filename = re.sub(r'[^/]*/(.*)', r'\1', line.strip())
            filename = os.path.join(hmdir, filename)
            file_list.append(filename)
            print("  \033[94m{}\033[0m.".format(filename))
        outprefix = stu_id + re.sub(r'([^/]*)/.*', r'\1', line.strip())

    print("")

    mr_file = os.path.join(hmdir, FILENAME_MUST_REMOVE)
    mrlist = []
    if os.path.isfile(mr_file):
        with open(mr_file, "r") as f:
            print("\033[93mExcluding: \033[0m")
            for line in f:
                filename = re.sub(r'[^/]*/(.*)', r'\1', line.strip())
                filename = os.path.join(hmdir, filename)
                print("  \033[93m{}\033[0m.".format(filename))
                mrlist.append(filename)
    else:
        print("\033[93m{} doesn't exist. Ignoring...\033[0m".format(FILENAME_MUST_REMOVE))

    print("")

    losts = [x for x in file_list if (not os.path.isfile(x)
                                      and not os.path.isdir(x))]
    if losts:
        for file in losts:
            print("\033[91m\033[01m{} should exist.\033[0m".format(file))
        return 1

    tarname = "{}.tgz".format(outprefix)
    tarpath = os.path.join(hmdir, tarname)
    tar = tarfile.open(tarpath, "w:gz")
    for filename in file_list:
        outname = re.sub(hmdir, outprefix, filename)
        tar.add(filename, outname, exclude=lambda name: check_remove(mrlist, name))

    tar.close()
    print('\033[92mSuccessfully created "\033[01m{}\033[0m\033[92m"!\033[0m'.format(tarpath))

    # run selfCheck if found
    if os.path.isfile(os.path.join(hmdir, FILENAME_SELFCHECK)):
        print("Running selfCheck...")
        os.chdir(hmdir)
        os.system("./{} {}".format(FILENAME_SELFCHECK, tarname))
    else:
        print("\033[93mCannot find selfCheck script, skipping.\033[0m")

    return 0

if __name__ == "__main__":
    args = parser.parse_args()
    ret = bundle(args.student_id, args.directory)
    exit(ret)
