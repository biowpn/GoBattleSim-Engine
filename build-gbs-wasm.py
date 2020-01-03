
import os
import shutil
import subprocess


def main():

    SourceDir = "src/"
    BinDir = "build-wasm"

    if os.path.isdir(BinDir):
        shutil.rmtree(BinDir)
    os.mkdir(BinDir)

    procs = []

    bin_paths = []

    for src in os.listdir(SourceDir):
        src_name, ext = src.split('.')
        if ext not in ["cpp", "cc"]:
            continue
        src_path = os.path.join(SourceDir, src)
        bin_path = os.path.join(BinDir, src_name + ".bc")
        cmd = f"em++ --std=c++11 -O2 -Iinclude -Ithirdparty -c {src_path} -o {bin_path}"
        bin_paths.append(bin_path)
        print(cmd)
        p = subprocess.Popen(cmd, shell=True)
        procs.append(p)

    for p in procs:
        p.wait()
        if p.returncode != 0:
            print("error occured")
            exit()

    bin_paths_str = ' '.join(bin_paths)
    cmd = f"emcc {bin_paths_str} -o {BinDir}/GBS.html -s EXTRA_EXPORTED_RUNTIME_METHODS=[ccall,cwrap]"
    print(cmd)
    p = subprocess.Popen(cmd, shell=True)
    p.wait()


if __name__ == "__main__":
    main()
