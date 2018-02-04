#!/usr/bin/env python3
import subprocess
import argparse
import shutil
import tempfile
import os


parser = argparse.ArgumentParser()
parser.add_argument('--rpcgen')
parser.add_argument('--infile')
parser.add_argument('--outdir')
parser.add_argument('--mode', choices=('header', 'source'))
flags = parser.parse_args()


target_suffixes = {
    'h': '.h',
    'c': '_xdr.c',
    'l': '_clnt.c',
    'm': '_svc.c',
}


def generate(target):
    infile_basename = os.path.basename(flags.infile)
    name, ext = os.path.splitext(infile_basename)
    with tempfile.TemporaryDirectory() as tempdir:
        shutil.copy(flags.infile, os.path.join(tempdir, infile_basename))
        command = [
            flags.rpcgen,
            infile_basename,
            '-N',
            '-' + target,
        ]
        with open(
            os.path.join(
                os.path.abspath(flags.outdir),
                name + target_suffixes[target]),
            'wb'
        ) as outf:
            subprocess.check_call(command, cwd=tempdir, stdout=outf)


if flags.mode == 'header':
    generate('h')
else:
    for target in 'lmc':
        generate(target)
