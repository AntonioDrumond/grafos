from PIL import Image
import sys

if len(sys.argv) != 2:
    print("Por favor informe o arquivo_de_entrada.png")
else:
    input_file = sys.argv[1] 
    try:
        imagem = Image.open(input_file) 
        if imagem.mode != 'RGB': 
            imagem = imagem.convert('RGB')
        
        imagem.save("input.ppm", format='PPM') 
    except Exception as e:
        print(f"Erro ao converter imagem: {e}")
