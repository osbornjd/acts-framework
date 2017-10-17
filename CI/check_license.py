#!/usr/bin/env python3
import argparse
import os
import sys
from subprocess import check_output
import re
import difflib
from datetime import datetime


def main():
    p = argparse.ArgumentParser()
    p.add_argument("input")
    p.add_argument("--fix", action="store_true")

    args = p.parse_args()

    if os.path.isdir(args.input):
        srcs = str(check_output(["find", args.input, "-iname", "*.cpp", "-or", "-iname", "*.hpp", "-or", "-iname", "*.ipp"]), "utf-8").strip().split("\n")
    else:
        srcs = [args.input]

    year = datetime.now().strftime("%Y")

    raw = """// This file is part of the ACTS project.
//
// Copyright (C) {year} ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/."""

    rawstr = raw.format(year=year)
    raw = rawstr


    reg = (
        r"\A// This file is part of the ACTS project.\n"
        +r"//\n"
        +r"// Copyright \(C\) 20[0-9]{2} ACTS project team\n"
        +r"//\n"
        +r"// This Source Code Form is subject to the terms of the Mozilla Public\n"
        +r"// License, v\. 2\.0\. If a copy of the MPL was not distributed with this\n"
        +r"// file, You can obtain one at http://mozilla.org/MPL/2.0/.\Z"
        )

    ref = re.compile(reg, re.M)
    clean_re = re.compile(r"20[0-9]{2}")
    def clean(s):
        return clean_re.sub("XXXX", s)
    def get_clean_lines(s):
        return [clean(l)+"\n" for l in s.split("\n")]
    raw = get_clean_lines(raw)

    exit = 0
    for src in srcs:
        with open(src, "r+") as f:
            license = ""
            for x in range(len(raw)):
                line = f.readline()
                if not line.startswith("//"):
                    break
                license += line
            license = ("".join(license)).strip()
            m = ref.search(license)
            if m == None:
                sys.stderr.write("invalid / mising license in "+src+"\n")
                diff = difflib.unified_diff(raw, get_clean_lines(license))
                sys.stderr.writelines(diff)
                sys.stderr.write("\n")

                if args.fix:
                    print("-> fixing file")
                    f.seek(0)
                    file_content = f.read()
                    f.seek(0)
                    f.write(rawstr+"\n\n")
                    f.write(file_content)

                exit = 1

    sys.exit(exit)



if "__main__" == __name__:
    main()
