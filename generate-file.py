import os
import random
import string


def generate_random_content(size_in_mb):
  size_in_bytes = size_in_mb * 1024 * 1024
  characters = string.ascii_letters + string.digits + string.punctuation
  return ''.join(random.choices(characters, k=size_in_bytes))

def write_to_file(filename, content):
  with open(filename, 'w') as file:
    file.write(content)

if __name__ == "__main__":
  filename = "random_content.txt"
  content = generate_random_content(100)
  write_to_file(filename, content)
  print(f"File '{filename}' with random content of size 100MB has been created.")
