# Book Resume

## Part 1 - Shared-Variable Programs
Pg. 39
Programas sequenciais comumente utilizam  variáveis compartilhadas por uma questão de conveniência, como estruturas de dados que são utilizadas em diversas partes de um programa. Entretanto um programa pode ser escrito sem fazer uso dessas variávis globais, e muitos defenderiam que isso seria uma boa prática.
Programas concorrentes, por outro lado, dependem fortemente do uso de variáveis compartilhadas. Isso por que **a unica forma que vários processos tem de trabalhar juntos é se comunicando**, e a unica forma de fazer isso é se um processo escreve em um local que outro processo irá ler. Isso pode ser feito em uma variável compartilhada ou em um canal de comunicação. Além disso essa comunicação é programada escrevendo e lendo variáveis compartilhadas ou mandando e recebendo mensagens.
Essa comunicação nos leva ao problema da sincronização, que é resolvida de duas formas: **exclusão mútua e condição de sincronização**. Exclusão mútua ocorre quando dois processos precisam acessar um recurso compartilhado e é preciso que exista uma coordenação nesse acesso, e condição de sincronização acontece quando um processo precisa esperar por outro.

### Capítulo 2 - Process and Synchronization
Pg. 41
Programas concorrentes são inerentemente mais complexos do que programs sequenciais. Em muitos aspectos os programas sequencias estão para os concorrentes, assim como o jogo de damas está para o xadrez. Podemos fazer uma analogia do jogo de xadrez com a programação concorrente, onde as **regras** são as ferramentas formais que nos ajudam a entender e desenvolver programas corretos; as peças são os mecanismos da linguagem para descrever a computação concorrente; a estratégia são as técnicas de programação úteis para resolver o problema.

Ter uma abordagem disciplinada com programas sequencias é importante, porém com programas concorrentes é indispensável, por que a ordem de execução das instruções é não-determinística. 

#### 2.1 - States, Actions, Histories and Properties
O estado de um programa concorrente é o **conjunto de valores que suas variaveis tem em um determinado momento do tempo** Em um programa teremos diversos processos  que executam de forma independente e que podem alterar os estados internos do programa.
Um processo executa uma sequencia de comandos, e esses comandos podem ser divididos em sequências de uma ou mais **instruções atômicas**, ações indivisíveis que leem ou alteram os estados do programa.
A execução de um programa concorrente leva ao **entrelaçamento** das intruções atômicas, executadas por cada processo, e um exemplo de entrelaçamento é chamado de **histórico de execução**. Mesmo execuções paralelas podem ser modeladas como uma história linear, por que o efeito de executar um conjunto de ações em paralelo é equivalente a **executa-los em uma sequênca linear**. A quantidade de históricos diferentes de execução que um programa simples pode ter e enorme, quando falamos de programas mais complexos com milhares de linhas de código e variáveis a descrição de todos seus históricos possíveis torna-se impraticavel computacionalmente.
A preocupação com a sincronização de determinadas ações acaba sendo uma medida paliativa para diminuir a infinidade de históricos possíveis para um conjunto que consigamos ter controle. A exclusão mutua pode ser implementada diretamente no hardware, construido operações atômicas, a sequência dessas ações é chamada de sessão crítica. Enquanto a sincronização de condição se preocupa com o atraso de uma determinada ação até que a condição desejada seja alcansada.
Na página 43 temos o detalhamento das propriedades de segurança e vitalidade.
A forma escolhida pelo autor para demonstrar que os algoritmos são corretos é através do raciocínio assertional, uma análise abstrata onde asserções são feitas para caracterizar estados do programa e as ações atômicas são vistas como transformações de predicados.

#### 2.2 - Parallelization: Finding Patterns in a File
Considere o problema de procurar um **pattern** em um arquivo de entrada, dado um nome. Esse problema é resolvido no Unix pelo comando **grep** com uma solução parecida coma  seguinte:

```bash
String line;
#read a line of input from stdin to line;
while(!EOF) {
	#look for a partten in line;
  if (pattern is in line)
  	write line
  #read next line of input;
}
```
O programa acima pode ser paralelizado? A exigência inicial para que um programa possa ser paralelizado é que ele contenha partes independentes, como descrito na seção **1.4**. Duas partes serão dependentes se o resultado produzido por uma for usado pela outra, por outro lado elas serão independentes se não forem dependentes  de variáveis compartilhadas. Matemáticamente podemos descrever como:

- **Independência de processos paralelos:** Seja **R o conjunto das variáveis que são apenas lidas no segmento S1 do programa** e seja **W o conjunto das variáveis que são alteradas, e possivelmente lidas, no segmento S1 do programa**. S1 e S2 serão independentes se a intersecção de Ws1 e Ws2 for vazio.

Voltando ao problema anterior podemos paralelizar da seguinte forma:

```bash
String line;
#read a line of input from stdin to line;
while(!EOF) {
	co	#look for a partten in line;
  		if (pattern is in line)
  			write line
  //	#read next line of input;
  oc
}
```
Nessa abordagem os dois processos não são independentes pois o primeiro le a variável **line** e o segundo escreve nela. Temos o problema citado e um grande overhead de criar dois processos a cada iteração do loop. Uma forma de diminuir o overhead é tratar o problema como uma instância do problema do produtor-consumidor, e cada processo terá que dividir um buffer comum. O problem aqui é coordenar o correto acesso a esse buffer.

```bash
String buffer;
bool done = false;

co #process 1: find patterns
	String line1;
  while(true) {
  	#wait for buffer to be full or done to be true;
    if(done) break;
    # Politica de sincronização in
    line1 = buffer;
    # Politica de sincronização out
    signal that buffer is empty;
    look for a pattern in line1;
    if(pattern is in line1)
    	write line1;

  }
// #process 2: read new lines
	String line2;
  while(true) {
  	read next line of input into line2;
    if(EOF) {done = true; break;}
    wait for buffer to be empty
    # Politica de sincronização in
    buffer = line2;
    # Politica de sincronização out
    signal that buffer is full;
  }
oc
```

#### 2.3 - Synchronization: The Maximum of an Array
Considere o seguinte problema: Temos um array de tamanho N com números inteiros positivos e queremos encontrar o valor máximo desse array, ou seja o objetivo desse programa é achar um **m** que satisfaça a seguinte expressão lógica:

```bash
(para todo j: 1 <= j <= n: m >= a[j]) ^ 
(existe um j: 1 <= j <= n: m == a[j])

# Expressando de forma algoritmica temos
int m = 0
for(i = 0; i < n; i++) {
	if(a[i] > m)
  	m = a[i]
}

# Totalmente paralelizado teriamos
int m = 0
co(i = 0 to n-1) {
	if(a[i] > m)
  	m = a[i]
}
```
Esse programa é incorreto por que mesmo que a leitura e a comparação de cada processo seja paralelizado a escrita, por ser atômica, será sequencial, fazendo com que m tenha qualquer valor. A solução mais natural é coordenar a execução do if de forma sequencial, fazemos isso abstratamente com o operador **<>**. Isso é problemático por que transforma o programa paralelo em sequencial.
```bash
int m = 0
co(i = 0 to n-1) {
	<if(a[i] > m)
  	m = a[i]>
}
```

#### 2.4 - Atomic Actions and Await Statements
Como foi dito anteriormente, a execução de um programa concorrente pode ser visto como o entrelaçamento de operações atômicas que são executadas por processos individuais. Quando esses processos interagem entre si, nem todos os entrelaçamentos são corretos. O papel da sincronização é lidar com entrelaçamentos indesejados. Isso é obtido através da combinação de ações atômicas de baixa granualidade com ações de alta granularidade ou atrasando um processo até que uma condição seja satisfeita. A primeira abordagem é chamada **Exclusão mútua**, e a segunda **Condição de sincronização**.

##### 2.4.1 - Fine-Grained Atomicity
Uma ação atômica de baixa granularidade é aquela que é implementada diretamente pelo hardware que executa o programa concorrente. Por exemplo

```bash
# a instrução
x++
# pode ser implementada a nível de máquina como
mov eax, x
add eax, 1
mov x, eax
```
Se não conseguimos obter a propriedade NMUV temos que construir ações atômicas de granularidade alta, que consiste em uma sequências de ações atômicas que parecerá indivisível.

#### 2.5 - Producer/Consumer Synchronization
Podemos resolver o problema do produtor consumidor da seguinte forma, considere a tarefa de copiar um array de elementos produzidos, do produtor para o consumidor:

```bash
int buf, p = 0, c = 0;
process Producer {
	int a[n];
  while(p < n) {
  	<await (p == c);>
    buf = a[p];
    p = p+1;
  }
}

process Consumer {
	int b[n];
  while(c < n) {
  	<await (p > c);>
    b[c] = buf;
    c = c+1;
  }
}
```
O statement ```<await (p>c)>``` pode ser implementado como ```while(!p > c)```, e esse tipo de trava é chamado de spinning ou busy waiting, por que ele está a todo tempo executando computação e verificando a condição booleana.

Na seção 2.6 temos uma extensa explicação de como utilizar lógica formal para provar a correção dos programas.

#### 2.8 - Safety and Liveness Properties
Como dito anteriormente a propriedade de um programa é um atributo que se mantem verdadeiro em todoas os históricos possiveis do programa, e as propriedades como segurança ou vitalidade. Em programas sequencias o ponto chave da propriedade de segurança é que o estado final do programa é correto, e da vitalidade é que o programa irá terminar. Podemos citar como propriedades de segurança a exclusão mútua e a ausencia de deadlock. Propriedades de vitalidade poderiam ser a entrada garantida na seção crítica em um dado momento da execução, propriedade a qual pode ser afetada pelas políticas do escalonador.


#### 2.8.2 - Scheduling Policies and Fairness
Grande parte das propriedades de vitalidade dependem de justiça no escalonamento, o que diz respeito a se um processo tem chance de ser processado, independente do que outros processos fazem. Cada processo executa uma sequência de instruções atômicas, e essas serão **elegíveis** se a próxima instrução puder ser executada.
Considere o seguinte exemplo:

```bash
bool continue = true;
co while (continue);
	// continue = false;
oc
```
Considerando que o escalonador escolheu o processo 1 para ser executado primeiro, e que não existe preempção, neste caso o programa nunca irá acabar. Podemos definir que uma política de escalonamento exerce **justiça incondicional** se toda ação atômica incondicional que é elegível for eventualmente executada.

Considere que a política do escalonador é round-robin, atribuindo a cada processador um processo e permitindo que ele seja executando durante um período de tempo. Nesse caso temos justiça incondicional.

Se temos comandos como ```<await>``` precisamos de afirmações mais fortes que garantam a execução. **Justiça fraca** é uma política de escalonamento onde:

1. É incondicionalmente justa
2. Toda ação atômica condicional é executada eventualmente, considerando que sua condição seja verdadeira e se mantenha verdadeira até que o processo que depende da condição seja escalonado.

**Justiça forte** é uma política de escalonamento onde:

1. É incondicionalmente justa
2. Toda ação atômica condicional que é elegivel é executada eventualmente, considerando que sua condição é infinitamene muitas vezes verdade.

É quase impossível ter uma política de escalonamento prática e que seja fortemente justa.

### Capítulo 3 - Locks and Barriers

#### 3.1 - The Critical Section Problem
O problema da seção crítica é um dos mais clássicos problemas dentro da programação concorrente. Neste problema, **n** processos executam seções não críticas e seções críticas repetidas vezes. Para entrar em uma seção crítica deve ser executado um **protocolo de entrada** e para sair um **protocolo de saída**. Exemplo:

```bash
process CS [i = 0 to n] {
	while(true) {
  	# Protocolo de entrada
    # SC
    # Protocolo de saída
    #SNC
  }
}
```

Nosso desafio é desenvolver um protocolo de entrada e saída que respeite as seguintes características:

1. Exclusão mutua - Apenas um processo por vez está executando a SC.
2. Ausência de Deadlock/Livelock - Se dois ou mais processos estão tentando entrar na SC, pelo menos um deles irá conseguir.
3. Ausência de espera desnecessária - Se um processo está tentando entrar na SC e outro processo está executando sua SNC ou terminou sua execução, o primeiro não deve ser impedido de entrar na SC.
4. Entrada garantida - Um processo que esperar entrar na sua SC eventualmente irá conseguir entrar.

As três primeiras propriedades são de segurança, e a ultima de vitalidade.
Uma solução para o problema da seção crítica para dois poderia ser a seguinte:

```bash
bool ini1 = false, ini2 = false;
## Mutex: !(ini1 ^ ini2) -- Invariante global
process CS1 {
	while(true) {
  	<await (!ini2) ini1 = true> # Protocolo de entrada
    SC
    ini1 = false # Protocolo de saída
    SNC
  }
}
process CS2 {
	while(true) {
  	<await (!ini1) ini2 = true> # Protocolo de entrada
    SC
    ini2 = false # Protocolo de saída
    SNC
  }
}
```

#### 3.2 Critical Sections: Spin Locks
A solução do problema anterior foi idealizada para apenas dois processos, como generalizar para n processos? Teriamos que ter n variáveis ? De certa forma sim, mas podemos considerar que existirão apenas dois estados do nosso interesse: algum processo está na região crítica e se nenhum processo está nela.
Processadores modernos implementam a nível de hardware instruções para construir ações atômicas, como o **Test and Set(TS)**, que le e salva o valor da trava, altera para true e então retorna o valor original da trava, por exemplo:

```bash
bool TS(bool lock) {
  < 
  bool initial = lock;
  lock = true;
  return initial;
  >
}
```

A solução para o problema original fica:

```bash
bool lock = false;

process CS[i = 1 to n] {
	while(true) {
		while(TS(lock)) skip; # Protocolo de entrada
    SC
    lock = false # Protocolo de saída
    SNC
  }
}
```

O uso de uma trava em um while recebe onome de **spin-lock**, o processo fica constantemente rodando no loop(spining) enquanto espera que a trava seja liberada. O grande trade-off de um spin lock é a necessidade de um escalonador de implemente um justiça forte, para garantir entrada eventual, o que é impraticável de se fazer. Para corrigir isso precisamos de algum algoritmo que funcione com justiça fraca.

#### 3.3.1 Tie-Breaker Algorithm
O problema da solução crítica dado pelo algoritmo anterior é que a entrada do processo que está esperando não é garantida, um processo que acabou de sair da sua seção crítica pode entrar novamente. Um algoritmo que resolve esse problema de forma justa é o **Tie-Breaker(conhecido também como algoritmo de Peterson)**. A ideia do algoritmo é gravar em uma variável quem foi o ultimo processo a executar a SC e caso esse processo tente executar novamente ele cede a vez para o outro.

```bash
bool in1 = false, in2 = false;
int last = 1;

process CS1 {
	while(true) {
  	last = 1; in1 = true
    < await (!in2 or last == 2); >
    SC
    in1 = false # Protocolo de saída
    SNC
  }
}
process CS2 {
	while(true) {
		last = 2; in1 = true
 		## O await pode ser trocado por um while
 < await (!in1 or last == 1); >
    SC
    in2 = false # Protocolo de saída
    SNC
  }
}
```
Existe uma generalização do algoritmo no fim da seção. Esse algoritmo é complexo e difícil de entender, na prática outro algoritmo deve ser utilizado.

#### 3.3.2 The Ticket Algorithm
O algoritmo ticket se fundamenta na distribuição de senhas para os processos que chegarem, de forma serial. O processo com o menor número é atendido primeiro, seguido pelo seu sucessor.
```bash
int number = 1, next = 1, turn[1:n] = ([n] 0);

process CS[i = 0 to n] {
	while(true) {
  	< turn[i] = number; number = number + 1; >
    < await turn[i] ==  next >
    SC
    <next = next + 1>
    SNC
  }
}
```
Esse algoritmo é vantajoso por ser livre de deadlock e ter entrada garantida, mesmo que o tempo de ociosidade de um processo seja grande. O grande problema dele é ter um contador, que se for incrementado indeterminadas vezes pode causar um estouro aritmético, além disso nem todos os hardwares implementam as funções atômicas necessárias para esse algoritmo funcionar (Fetch-and-Add).

#### 3.3.3 The Bakery Algorithm
O algoritmo Bakery é muito semelhante ao Ticket, porém não necessita de nenhuma instrução específica de máquina para funcionar, além de ser justo.
```
int turn[1:n] = ([n] 0);

process CS[i = 0 to n] {
	while(true) {
  	< turn[i] = max(turn[1:n]) + 1; >
    for [j = 1 to n st j != i]
	    < await (turn[j] ==  0 or turn[i] < turn[j]) >
    SC
    turn[i] = 0;
    SNC
  }
}

```

#### 3.4 - Barrier Synchronization
Muitos algoritmos paralelos consistem em dividir o trabalho entre inumeros trabalhadores, calcular e devolver o valor para que a iteração seguinte seja calculada. Para que exista uma sincronização correta entre as iterações os processos tem que esperar que todos os outros terminem seus calculos para poderem prosseguir, e isso pode ser feito utilizando uma barreira de sincronização.

O ideal é ter n processos trabalhadores e 1 processo coordenador, que irá sinalizar para os demais trabalhadores quando devem recomeçar a execução, montar os dados da proxima iteração e etc...
