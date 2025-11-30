from PIL import Image
import sys

if len(sys.argv) != 3:
    print("Por favor informe o arquivo_de_entrada.png e o arquivo_de_saida.ppm")
else:
    input_file = sys.argv[1] 
    output_file = sys.argv[2]  
    # convert_png_to_ppm(input_file, output_file) 
    try:
        imagem = Image.open(input_file) 
        
        if imagem.mode != 'RGB': 
            imagem = imagem.convert('RGB')
        
        imagem.save(output_file, format='PPM') 
    except Exception as e:
        print(f"Erro ao converter imagem: {e}")
