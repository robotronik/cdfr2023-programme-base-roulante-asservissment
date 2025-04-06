import subprocess

# Obtenez le SHA du dernier commit
commit_sha = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"]).strip().decode()

# Découpez le SHA en 4 parties de 2 caractères chacune
part1 = commit_sha[:2]
part2 = commit_sha[2:4]
part3 = commit_sha[4:6]
part4 = commit_sha[6:8]

# Affichez les parties sous forme hexadécimale pour PlatformIO
print(f"-DGIT_COMMIT_SHA_PART1=0x{part1}")
print(f"-DGIT_COMMIT_SHA_PART2=0x{part2}")
print(f"-DGIT_COMMIT_SHA_PART3=0x{part3}")
print(f"-DGIT_COMMIT_SHA_PART4=0x{part4}")