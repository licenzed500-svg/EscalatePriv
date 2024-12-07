import os
import subprocess

def get_libraries():
    libraries = set()
    for root, dirs, files in os.walk('/'):
        for file in files:
            if file.endswith('.so') or file.endswith('.so.*'):
                file_path = os.path.join(root, file)
                libraries.add(file_path)
    return libraries

def get_library_versions(libraries):
    library_versions = {}
    for library in libraries:
        try:
            output = subprocess.check_output(['ldd', library], stderr=subprocess.STDOUT).decode('utf-8')
            for line in output.splitlines():
                parts = line.split('=>')
                if len(parts) == 2:
                    lib_path = parts[1].strip().split(' ')[0]
                    if lib_path not in library_versions:
                        library_versions[lib_path] = subprocess.check_output(['strings', lib_path], stderr=subprocess.STDOUT).decode('utf-8').strip()
        except Exception as e:
            print(f"Error processing {library}: {e}")
    return library_versions

def main():
    libraries = get_libraries()
    library_versions = get_library_versions(libraries)
    for lib, version in library_versions.items():
        print(f"{lib}: {version}")

if __name__ == '__main__':
    main()
