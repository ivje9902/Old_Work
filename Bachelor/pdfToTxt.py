import pdfplumber
import os

path = "./material/data"
# Check if the directory exists
if not os.path.exists(path):
    print(f"The directory {path} does not exist.")
    exit(1)
# Check if the directory is empty
if not os.listdir(path):
    print(f"The directory {path} is empty.")
    exit(1)
# Function to convert PDF to text
# This function takes a PDF file path as input and converts it to a text file
# with the same name but with a .txt extension.
# It uses the pdfplumber library to extract text from each page of the PDF
# and writes it to the text file.

def pdf_to_text(pdf_path):
    txtpath = pdf_path.replace('.pdf', '.txt')
    with pdfplumber.open(pdf_path) as pdf, open(txtpath, 'w', encoding='utf-8') as txt_file:
        for page in pdf.pages:
            text = page.extract_text()
            if text:
                txt_file.write(text + '\n') 


for file in os.listdir(path = path):
    if file.endswith('.pdf'):
        print(f"Converting {file} to text...")
        file = os.path.join(path, file)
        pdf_to_text(file)
        print(f"Converted {file} to text.")


