## GPGPU

### Panorama histórico
As placas de gráticas de alto desempenho tinham basicamente a função de converter a descrição de uma cena em uma imagem. Para isso a cena era descrita em função de triângulos, texturas, iluminação e etc. Para essa representação a placa operava com um pipeline de três funções básicas:
- Processamento dos vértices dos triângulos
- Varedura da cena
- Aplicação de textura cor e sombreamento

Para executar todas essas atividades um intenso cálculo matemático era necessário. Naquela época as placas tinham unidades funcionais específicas para cada coisa, fazendo com que fossem de propósito único.
Com o passar dos anos percebeu-se que uma GPU poderia ser empregada em processamento numérico e científico. Em mais tarde nasce a GPGPU (General Purose Graphics Processing Unit).

A GPU é conectada ao computador através de um barramento **PCI Express**. Um cálculo básico da GPU segue os seguintes passos:

1. Alocação de espaço na memória da GPU e transferência de dados
2. Transferênca da execução para o kernel da GPU
3. Os resultados ficam na memória interna da GPU
4. Os resultados são transferidos para a CPU e o ciclo pode recomeçar do passo 1

Uma GPU funciona sobre o regime da SIMT (Single instruction multiple threads), onde multiplas threads executam o mesmo código sobre **um conjunto de dados diferentes**

### Hierarquia de memória
1. Memória local = Thread
2. Memória compartilhada = Bloco de Threads
3. Memória global = Entre Blocos

- Thread = Unidade de programação
- Warp = Unidade de execução
- Bloco = Unidade de alocação


### Cuda
No Cuda a GPU é um coprocessador que executa funções paralelas. Temos a seguinte divisas:

GRID -> BLOCO -> THREAD

Por ter diversos fabricantes e interfaces de programação existe o **OpenCL**, que é uma tentativa de criar uma API unificada 