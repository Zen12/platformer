"""
This code is write by chatGTP, this is not expected to be mantaine
Purpose of this script is to automate manual work

What it does:
 - Deletes all the .meta files
"""

import os
import uuid

"""TODO: THIS IS HARDCODED. Make it as param or dynamic from project.yaml"""
directory = 'assets/resources'


for root, dirs, files in os.walk(directory):
    for file in files:
        if file.endswith('.meta'):
            file_path = os.path.join(root, file)
            try:
                os.remove(file_path)
                print(f"Deleted: {file_path}")
            except Exception as e:
                print(f"Failed to delete {file_path}: {e}")