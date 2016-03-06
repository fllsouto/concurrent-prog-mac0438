# Resumo Programação Concorrente

### Conceituando os diferentes tipos de programação com processos

#### Concorrente
- Vários processos(threads) compartilham a CPU
- Modelo de memória compartilhada

#### Distribuída
- Processamento distribuído entre máquinas conectadas a uma rede comum
- Modelo de troca de mensagens

#### Paralela
- Processos concorrentes executam em processadores distintos
- Modelo de memória compartilhada ou troca de mensagens

Um processador com hyperthread continua tendo apenas um núcleo, porém o escalonamento das instruções pode ser feito a nível muito baixo, de instruções funcionais. Essa técnica é vantajosa e pode aumentar cerca de 10% a velocidade do processamento das instruções.