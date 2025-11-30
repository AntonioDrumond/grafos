from PIL import Image
import sys

if len(sys.argv) != 3:
    print("Por favor, informe o arquivo_de_entrada.ppm e o arquivo_de_saida.png")
else:
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    img = Image.open(input_file)
    img.save(output_file, 'PNG')
