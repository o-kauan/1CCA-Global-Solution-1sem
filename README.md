# Sistema de Monitoramento de Missão Espacial

## Instruções para Execução do programa

Siga as diretrizes abaixo para baixar o código-fonte diretamente do repositório público e executá-lo em seu ambiente local:

### Passo 1: Clonagem do Repositório

Abra o terminal de comandos de sua preferência e realize a clonagem utilizando a URL do repositório:

```bash
git clone https://github.com/o-kauan/1CCA-Global-Solution-1sem.git

```

### Passo 2: Navegação até o Diretório

Entre na pasta criada pelo Git para garantir que os comandos de compilação rodem no mesmo nível do arquivo principal:

```bash
cd 1CCA-Global-Solution-1sem

```

### Passo 3: Compilação do Código-Fonte

Utilize o compilador GCC para processar o arquivo `main.c` e gerar o binário executável do sistema:

```bash
gcc main.c -o sistema_espacial

```

### Passo 4: Execução do Programa

Inicie a aplicação de acordo com o sistema operacional utilizado:

* **Ambientes Windows:**
```bash
sistema_espacial.exe

```

* **Ambientes Linux ou macOS:**
```bash
./sistema_espacial

```
## 1. Introdução

Este documento descreve a arquitetura lógica do Sistema de Monitoramento de Missão Espacial desenvolvido em Linguagem C. O software opera como um núcleo de processamento em tempo real, projetado para simular o diagnóstico e a mitigação de falhas em uma espaçonave. Através de algoritmos de validação de fluxo, mapeamento binário de estados e busca em matrizes multidimensionais, o sistema assegura a consistência dos dados operacionais e automatiza tomadas de decisão críticas.

---

## 2. Visão Geral das Funcionalidades

O programa foi projetado para executar quatro tarefas principais de forma simples e direta através de um menu interativo:

* **Entrada Protegida de Dados:** Permite digitar as informações da nave (como temperatura e nível de bateria). O sistema ignora digitações erradas (como letras digitadas por engano no lugar de números) para impedir que o programa trave ou entre em loop infinito.
* **Exibição do Painel de Bordo:** Mostra um resumo organizado de todos os dados atuais e calcula se a nave está acumulando ou perdendo energia elétrica.
* **Diagnóstico Automático da Situação:** Avalia se os dados da nave estão dentro de limites seguros. Caso encontre problemas, calcula o tempo restante de bateria e define a gravidade geral da missão como Normal, Alerta ou Crítico.
* **Central de Soluções:** Se houver falhas, o sistema cruza os dados de energia com os problemas ativos para sugerir instruções exatas de reparo baseadas no contexto atual da espaçonave.

---

## 3. Análise Detalhada da Lógica por Opção do Menu

### Opção 1: Inserir Dados 

Esta seção é responsável pela ingestão de dados brutos e sua conversão para as variáveis globais do sistema. Devido à volatilidade do buffer de entrada padrão, a lógica emprega uma rotina rigorosa de validação:

* **Tratamento de Exceções com `filtrar_decimais_e_letras`:** Chamada consecutiva para validar dados do tipo inteiro (como `comunicacao_status` e `energia_porcent`). Se a função retornar 0 (falso), o fluxo é retido em estruturas de repetição (`while`) até que uma entrada válida seja fornecida.
* **Limpeza do Buffer pós-Leitura de Ponto Flutuante:** Para variáveis do tipo `float` (`temperatura`, `consumo_energia_kWh`, `geracao_solar_kW`), o sistema utiliza diretamente o `scanf`. Imediatamente após a validação do tipo, o comando `while (getchar() != '\n');` é executado para expurgar o caractere de nova linha (`\n`) ou qualquer resíduo remanescente no buffer, impedindo o atropelamento das leituras subsequentes.
* **Processamento de Derivados:** Ao fim da coleta, o sistema realiza operações aritméticas para definir a `energia_disponivel_kWh` com base na capacidade nominal fixa (1000.0 kWh) e calcula o `saldo_energia_kWh` como a diferença linear entre a geração e o consumo de energia.

### Opção 2: Visualizar Status

Atua como uma camada de apresentação de dados estruturados.

* **Controle de Fluxo por Flag:** Utiliza a variável booleana `inserir_dados` como uma trava de segurança. Se o valor for 0, o acesso é negado, prevenindo a exibição de lixo de memória.
* **Avaliação de Tendência Energética:** Através de condicionais simples, o sistema analisa o sinal do `saldo_energia_kWh`. Valores estritamente menores que zero classificam o estado como "Perdendo energia", valores maiores indicam "Recarregando energia" e zero indica "Energia estável".

### Opção 3: Executar Análise

Esta é a engrenagem central de diagnóstico. Toda vez que é acionada, o sistema limpa o vetor executando um laço que redefine todos os elementos de `status_op` para **0**. Em seguida, uma sequência de testes condicionais reavalia a telemetria e define os 7 índices:

| Índice do Vetor | Condição Ativada (Valor = 1) | Critério de Ativação no Código |
| --- | --- | --- |
| `status_op[0]` | Comunicação Inativa | Status de comunicação = 0. |
| `status_op[1]` | Superaquecimento |  Temperatura da nave > 80°C. |
| `status_op[2]` | Resfriamento Extremo | Temperatura da nave < -20°C. |
| `status_op[3]` | Baixa Energia (Economia) | Porcentagem da bateria < 20%. |
| `status_op[4]` | Perdendo Energia | Saldo de energia < 0 (consumo maior que geração). |
| `status_op[5]` | Recarregando Energia | Saldo de energia > 0 (geração maior que consumo). |
| `status_op[6]` | Energia Estável | Saldo de energia = 0. |

* **Lógica de Gravidade:** O programa soma apenas os valores de `status_op[0]` até `status_op[4]` (os índices de problemas). Se o resultado for maior ou igual a 2, o painel exibe estado **CRÍTICO**. Se for igual a 1 (e o problema não for apenas a perda de energia padrão com bateria alta), exibe **ALERTA**. Se for 0, o estado é **NORMAL**.

### Opção 4: Resolver Problemas

Após verificar se a análise (Opção 3) foi previamente processada através da flag `analise`, esta seção implementa o motor de tomada de decisão baseado no estado da nave, utilizando respostas pré-programadas da constante tridimensional `solucoes[2][4][3]`.

* **Cálculo de Índices:** O algoritmo determina as coordenadas exatas da matriz de soluções baseando-se na energia disponível e no saldo de energia:
* `idx_bateria`: Mapeado diretamente pelo valor de `status_op[3]` (0 para carga normal, 1 para baixa energia). Define qual plano horizontal da matriz será consultado.
* `idx_saldo`: Traduzido para os valores 0 (se houver perda), 1 (se houver recarga, verificado por `status_op[5]`) ou 2 (se houver estabilidade, verificado por `status_op[6]`).


* **Varredura e Extração de Respostas (Complexidade $O(1)$ por consulta):** Um loop itera sobre os três primeiros subsistemas (Comunicação, Superaquecimento e Resfriamento). Se o subsistema apresentar falha (`status_op[i] == 1`), o sistema acessa instantaneamente a string correspondente em `solucoes[idx_bateria][i][idx_saldo]`.
* **Injeção de Soluções de Escalonamento:** Se o modo de economia estiver ativo (`idx_bateria == 1`), o sistema utiliza o índice `[3]` da matriz, que contém as diretrizes de específicas de gerenciamento de energia.
* **Aviso de Sobrecarga:** Se a soma das falhas for igual a 4 e `status_op[4]` estiver ativo, o sistema faz uma leitura cruzada e acrescenta um alerta emergencial de apagão iminente na tela.

### Opção 0: Encerrar Sistema (Terminação de Fluxo)

Atualiza a variável de controle do laço `opcao` para 0. O predicado do `do-while` falha, quebrando a estrutura de repetição principal e retornando o controle ao sistema operacional através do `return 0`, garantindo a desalocação segura do frame de pilha da função `main`.

---

## 4. Arquitetura de Funções Auxiliares e Tratamento de Exceções

### Função `filtrar_decimais_e_letras`

```c
int filtrar_decimais_e_letras(int* resposta){
    if (scanf("%d", resposta) != 1) {
        while (getchar() != '\n'); 
        return 0; 
    }
    if (getchar() != '\n') {
        while (getchar() != '\n');
        return 0; 
    }
    return 1; 
}

```

Esta função implementa um filtro contra corrupção de fluxo de dados na leitura de inteiros:

1. O primeiro bloco `if` verifica o retorno do `scanf`. Se o retorno for diferente de 1, significa que o usuário inseriu caracteres não numéricos. O laço interno `while(getchar() != '\n')` consome e descarta esses caracteres do buffer de entrada para evitar travamento em loops infinitos.
2. O segundo bloco `if` trata entradas parciais textuais ou decimais (ex: o usuário digita `2.5` ou `2a`). O `scanf` captura com sucesso o inteiro `2`, mas deixa `.5` ou `a` no buffer. O `getchar()` captura o caractere imediatamente subsequente; se este não for uma quebra de linha (`\n`), a entrada é considerada inválida, o buffer é limpo e a função retorna 0.

### Função `exibir_tempo_formatado`

```c
void exibir_tempo_formatado(float horas) {
    if (horas < 0) { horas = -horas; }
    int total_segundos = (int)(horas * 3600.0f + 0.5f);
    int h = total_segundos / 3600;
    int m = (total_segundos % 3600) / 60;
    int s = total_segundos % 60;
    int d = h / 24;
    h = h % 24;
}

```

* **Prevenção de Sinais:** Força a conversão do parâmetro para um valor estritamente positivo.
* **Ajuste de Arredondamento por Ponto Flutuante:** A expressão `+ 0.5f` aplicada antes do *cast* para `int` resolve o problema de truncamento inerente à conversão de tipos em C. Isso garante que valores como `59.9999` segundos sejam arredondados corretamente para o inteiro superior mais próximo.
* **Decomposição Dimensional:** Baseia-se em operadores de divisão inteira (`/`) para extração de unidades principais e operadores de resto de divisão (`%`) para isolar os resíduos temporais escalares (convertendo a base decimal de horas para a base sexagesimal de minutos e segundos).
