Leitor e Gerador de Códigos de Barras em Imagens PBM

Este projeto foi desenvolvido para facilitar a geração e leitura de códigos de barras a partir de imagens no formato PBM. A proposta é utilizar o formato P1 para processar as imagens e extrair ou gerar informações codificadas de forma eficiente.
📋 Funcionalidades

    Gerador de Códigos de Barras: Gera imagens PBM com códigos de barras no formato compatível.
    Leitor de Códigos de Barras: Lê e decodifica informações contidas em imagens PBM.

🚀 Como Usar

    Faça o download do repositório. Certifique-se de baixar todos os arquivos disponíveis.

    Compile os arquivos fonte. Utilize um compilador C (como GCC) para compilar os códigos leitor.c e gerador.c:

gcc leitor.c -o leitor
gcc gerador.c -o gerador

Execute os programas.

    Para gerar um código de barras:

./gerador

Para ler um código de barras:

        ./leitor

    Siga as instruções exibidas no terminal para informar os dados necessários.

🛠️ Requisitos

    Compilador C: GCC ou similar.
    Formato da Imagem: Apenas imagens PBM do tipo P1 são aceitas para leitura.

👥 Desenvolvedores

    Alexandre Maciel
    Newton Ruck
