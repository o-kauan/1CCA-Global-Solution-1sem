#include <stdio.h>

int energia_porcent, opcao, comunicacao_status, status_op[7] = {0, 0, 0, 0, 0, 0, 0}, soma_status = 0, analise = 0, inserir_dados = 0;
float temperatura, geracao_solar_kW, capacidade_bateria_kWh = 1000.0, energia_disponivel_kWh, consumo_energia_kWh, saldo_energia_kWh;

const char *problemas[] = {
    "- Comunicacao inativa\n",
    "- Superaquecimento\n",
    "- Resfriamento extremo\n",
    "- Baixa energia\n",
    "- Perdendo energia\n",
};

// Matriz de respostas organizada por: 
// [0 = energia >= 20%, 1 = economia de energia]
// [0 = Comunicacao inativa, 1 = Superaquecimento, 2 = Resfriamento extremo, 3 = Gerenciamento de energia]
// [0 = Perdendo energia, 1 = Recarregando energia, 2 = Energia estavel]
const char *solucoes[2][4][3] = {
    // === MATRIZ 1: BATERIA COM CARGA (não está no modo economia de energia) ===
    {
        // [0] Comunicação Inativa
        {
            "[COMUNICACAO] Link offline. Mantendo varredura de sinal padrao e tentativas de reconexao.\n", 
            "[COMUNICACAO] Link offline. Iniciando varredura de alta potencia e reinicializacao dos transmissores principais.\n", 
            "[COMUNICACAO] Link offline. Iniciando diagnostico de hardware. Potencia de transmissao mantida nos niveis operacionais padrao.\n" 
        },
        // [1] Superaquecimento
        {
            "[TERMICO] Alerta de alta temperatura. Iniciando exaustores auxiliares e reduzindo ciclos do motor.\n", 
            "[TERMICO] Alerta de alta temperatura. Potencializando exaustores e bombas de refrigeracao para aceleracao do ciclo liquido.\n", 
            "[TERMICO] Alerta de alta temperatura. Ativando exaustores auxiliares e ciclo de refrigeracao nominal.\n" 
        },
        // [2] Resfriamento Extremo
        {
            "[TERMICO] Alerta de baixa temperatura. Desviando calor dissipado dos reatores e motores para as zonas habitaveis da nave.\n", 
            "[TERMICO] Alerta de baixa temperatura. Aquecedores internos acionados em capacidade maxima.\n", 
            "[TERMICO] Alerta de baixa temperatura. Acionando malha de aquecimento interno.\n" 
        },
        // [3] Gerenciamento de Energia (Não aplicável se há carga)
        { "", "", "" }
    },
    // === MATRIZ 2: BAIXA ENERGIA (modo economia de energia) ===
    {
        // [0] Comunicação Inativa
        {
            "[COMUNICACAO] Link offline. Transmissores totalmente desativados devido ao corte emergencial de subsistemas nao vitais.\n", 
            "[COMUNICACAO] Link offline. Fluxo de energia direto dos geradores priorizado para reativar o receptor basico de emergencia.\n", 
            "[COMUNICACAO] Link offline. Transmissores principais desativados. Sistema operando apenas em escuta passiva.\n" 
        },
        // [1] Superaquecimento
        {
            "[TERMICO] Alerta de alta temperatura. Diminuindo ciclos do motor; sistemas de arrefecimento operando em dissipacao passiva.\n", 
            "[TERMICO] Alerta de alta temperatura. Direcionando o pico de captacao atual para os dissipadores de calor ativos da fuselagem.\n", 
            "[TERMICO] Alerta de alta temperatura. Descarregando calor via radiadores mecanicos de emergencia.\n" 
        },
        // [2] Resfriamento Extremo
        {
            "[TERMICO] Alerta de baixa temperatura. Ativando barreiras fisicas de isolamento termico e acionando malha de aquecimento em zonas vitais.\n", 
            "[TERMICO] Alerta de baixa temperatura. Aquecedores internos acionados em capacidade maxima.\n", 
            "[TERMICO] Alerta de baixa temperatura. Acionando malha de aquecimento interno.\n" 
        },
        // [3] Gerenciamento de Energia
        {
            "[ENERGIA] Acionando modo ultra economia de energia e priorizando sistemas vitais. Iniciada manobra de dobra e calculando rota para fonte de enegia solar mais proxima.\n", 
            "[ENERGIA] Baixa energia, mas em recarga. Preparando jogos para passar o tempo.\n", 
            "[ENERGIA] Baixa energia, mas estavel. Iniciada manobra de dobra e calculando rota para fonte de enegia solar mais eficiente.\n" 
        }
    }
};

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

void exibir_tempo_formatado(float horas) {
    if (horas < 0) {
        horas = -horas;
    }
    int total_segundos = (int)(horas * 3600.0f + 0.5f);
    int h = total_segundos / 3600;
    int m = (total_segundos % 3600) / 60;
    int s = total_segundos % 60;
    int d = h / 24;
    h = h % 24;

    if (d > 0) {
        printf("%dd %dh %02dmin %02ds\n", d, h, m, s);
    } else {
        printf("%dh %02dmin %02ds\n", h, m, s);
    }
}

int main(){
    do {
    menu:
        printf("\n===========================================\n");
        printf("SISTEMA DE MONITORAMENTO DE MISSAO ESPACIAL");
        printf("\n===========================================\n");
        printf("(1) Inserir Dados\n");
        printf("(2) Visualizar Status\n");
        printf("(3) Executar Analise\n");
        printf("(4) Resolver Problemas\n");
        printf("(0) Encerrar Sistema\n");
        printf("===========================================\n");

        printf("Escolha uma opcao: ");
        if (filtrar_decimais_e_letras(&opcao) != 1 || opcao < 0 || opcao > 4) { 
            printf("\nOpcao invalida!\n");
            goto menu;
        }

        switch(opcao){
            case 1:
                printf("\n=== INSERIR DADOS ===\n");
                inserir_dados = 1;

                printf("Digite o status da comunicacao (1) - Ativa ou (0) - Inativa: ");
                while (filtrar_decimais_e_letras(&comunicacao_status) != 1 || (comunicacao_status != 0 && comunicacao_status != 1)) {
                    printf("========================================================\n");
                    printf("Status de comunicacao deve ser 1 (Ativa) ou 0 (Inativa)!");
                    printf("\n========================================================\n");
                    printf("Digite o status da comunicacao (1) - Ativa ou (0) - Inativa: ");
                }

                printf("Digite a temperatura (%cC): ", 248);
                while (scanf("%f", &temperatura) != 1 || temperatura < -273 || temperatura > 10000) {
                    while (getchar() != '\n');
                    printf("==============================================\n");
                    printf("Temperatura deve estar entre -273%cC e 10000%cC!", 248, 248);
                    printf("\n==============================================\n");
                    printf("Digite a temperatura (%cC): ", 248);
                }
                while (getchar() != '\n');

                printf("Digite a porcentagem de energia (%%): ");
                while (filtrar_decimais_e_letras(&energia_porcent) != 1 || energia_porcent < 0 || energia_porcent > 100) {
                    printf("==================================================\n");
                    printf("Porcentagem de energia deve estar entre 0%% e 100%%!");
                    printf("\n==================================================\n");
                    printf("Digite a porcentagem de energia (%%): ");
                }

                printf("Digite o consumo de energia (kWh): ");
                while (scanf("%f", &consumo_energia_kWh) != 1 || consumo_energia_kWh < 0 || consumo_energia_kWh > 10000) {
                    while (getchar() != '\n');
                    printf("======================================================\n");
                    printf("Consumo de energia deve estar entre 0 kWh e 10000 kWh!");
                    printf("\n======================================================\n");
                    printf("Digite o consumo de energia (kWh): ");
                }
                while (getchar() != '\n');

                printf("Digite a geracao de energia solar (kW): ");
                while (scanf("%f", &geracao_solar_kW) != 1 || geracao_solar_kW < 0 || geracao_solar_kW > 10000) {
                    while (getchar() != '\n');
                    printf("==========================================================\n");
                    printf("Geracao de energia solar deve estar entre 0 kW e 10000 kW!");
                    printf("\n==========================================================\n");
                    printf("Digite a geracao de energia solar (kW): ");
                }
                while (getchar() != '\n'); 

                energia_disponivel_kWh = capacidade_bateria_kWh * (energia_porcent / 100.0f);
                saldo_energia_kWh = geracao_solar_kW - consumo_energia_kWh;

                printf("\nDados inseridos com sucesso!\n");
                break; 

            case 2:
                printf("\n=== STATUS DA MISSAO ===\n");

                if (inserir_dados == 0) {
                    printf("Insira os dados para ter informações sobre o status da missão.\n");
                } else {
                    printf("Comunicacao: ");
                    if (comunicacao_status == 1) printf("Ativa\n"); else printf("Inativa\n");
                    printf("Temperatura: %.2f%cC\n", temperatura, 248);
                    printf("Capacidade maxima da bateria: %.0f kWh\n", capacidade_bateria_kWh);
                    printf("Energia disponivel: %d%% (%.2f kWh)\n", energia_porcent, energia_disponivel_kWh);
                    printf("Consumo de energia: %.2f kWh\n", consumo_energia_kWh);
                    printf("Geracao de energia solar: %.2f kW\n", geracao_solar_kW);

                    if (saldo_energia_kWh < 0) {
                        printf("Perdendo energia: (%.2f kWh)\n", saldo_energia_kWh);
                    } else if (saldo_energia_kWh > 0) {
                        printf("Recarregando energia: (%.2f kWh)\n", saldo_energia_kWh);
                    } else {
                        printf("Energia estavel\n");
                    }
                }
                break;

            case 3:
                printf("\n=== ANALISE DA SITUACAO ===\n");

                if (inserir_dados == 0) {
                    printf("Insira os dados para ter a analise da situacao.\n");
                } else {
                    analise = 1;
                    for (int i = 0; i < 7; ++i) status_op[i] = 0; 

                    printf("Comunicacao: ");
                    if (comunicacao_status == 1) {
                        printf("Ativa - OK\n");
                    } else {
                        printf("Inativa - Contanto com a Terra perdido!\n");
                        status_op[0] = 1;
                    }

                    printf("Temperatura: %.2f%cC", temperatura, 248);
                    if (temperatura > 80) {
                        printf(" - Superaquecimento!\n");
                        status_op[1] = 1;
                    } else if (temperatura < -20) {
                        printf(" - Resfriamento extremo!\n");
                        status_op[2] = 1;
                    } else {
                        printf(" - OK\n");
                    }

                    printf("Energia: %.2f kWh (%d%%)", energia_disponivel_kWh, energia_porcent);
                    if (energia_porcent < 20) {
                        printf(" - Economia de energia\n");
                        status_op[3] = 1;
                    } else {
                        printf(" - OK\n");
                    }
                    
                    if (saldo_energia_kWh < 0) {
                        status_op[4] = 1;
                        printf("Saldo de energia: %.2f kWh (Perdendo energia)\n", saldo_energia_kWh);
                        printf("Tempo de bateria restante: ");
                        exibir_tempo_formatado((energia_disponivel_kWh / saldo_energia_kWh) * -1); 

                    } else if (saldo_energia_kWh > 0) {
                        status_op[5] = 1;
                        printf("Saldo de energia: %.2f kWh (Recarregando energia)\n", saldo_energia_kWh);
                        printf("Tempo para recarregar completamente: ");
                        exibir_tempo_formatado((capacidade_bateria_kWh - energia_disponivel_kWh) / saldo_energia_kWh);

                    } else {
                        status_op[6] = 1;
                        printf("Saldo de energia: %.2f kWh (Energia estavel)\n", saldo_energia_kWh);
                    }

                    soma_status = 0;
                    for (int i = 0; i < 5; i++) {
                        soma_status += status_op[i];
                    }

                    printf("= Status Geral da Missao: ");
                    if (soma_status >= 2) {
                        printf("CRITICO =\n");
                    } else if (soma_status == 1 && status_op[4] == 0) {
                        printf("ALERTA =\n");
                    } else {
                        printf("NORMAL=\n");
                    }
                }
                break;

            case 4:
                printf("\n=== RESOLVER PROBLEMAS ===\n");
                if (analise == 0) {
                    printf("Realize a analise da situacao para identificar problemas.\n");
                    break;
                } 
                else if (soma_status == 0 || (soma_status == 1 && status_op[4] == 1 && status_op[3] == 0)) {
                    printf("Nenhum problema identificado. Missao operando normalmente.\n");
                    break;
                } 
                else { 
                    printf("\n== Problemas Identificados ==\n");

                    for (int i = 0; i < 5; ++i) {
                        if (status_op[i]) {
                            printf("%s", problemas[i]);
                        }
                    }
                    
                    printf("\n== Situacao da Bateria ==\n");

                    printf("Energia disponivel: %d%% (%.2f kWh)\n", energia_porcent, energia_disponivel_kWh);
                    if (saldo_energia_kWh < 0) {
                        printf("Saldo de energia: %.2f kWh (Perdendo energia)\n", saldo_energia_kWh);
                        printf("Tempo de bateria restante: ");
                        exibir_tempo_formatado((energia_disponivel_kWh / saldo_energia_kWh) * -1);

                    } else if (saldo_energia_kWh > 0) {
                        printf("Saldo de energia: %.2f kWh (Recarregando energia)\n", saldo_energia_kWh);
                        printf("Tempo para recarregar completamente: ");
                        exibir_tempo_formatado((capacidade_bateria_kWh - energia_disponivel_kWh) / saldo_energia_kWh);
                        
                    } else {
                        printf("Saldo de energia: %.2f kWh (Energia estavel)\n", saldo_energia_kWh);
                    }

                    printf("\n== Solucoes Sugeridas ==\n");

                    // Aproveita o saldo de energia também como o 3º índice da matriz: 0 = Perdendo, 1 = Recarregando, 2 = Estável
                    int idx_saldo = 0;
                    if (status_op[5]) idx_saldo = 1;
                    else if (status_op[6]) idx_saldo = 2;

                    // Define o 1º indice (qual matriz será utilizada) baseado no status_op[3] (0 = com carga, 1 = baixa energia)
                    int idx_bateria = status_op[3];
                    
                    // Imprime as soluções para Comunicação (0), Superaquecimento (1) e Resfriamento (2)
                    for (int i = 0; i < 3; i++) {
                        if (status_op[i]) {
                            printf("%s", solucoes[idx_bateria][i][idx_saldo]);
                        }
                    }

                    // Exibe soluções para baixa energia, mas somente quando ela está em modo de economia de energia
                    if (idx_bateria == 1) {
                        printf("%s", solucoes[1][3][idx_saldo]);
                    }

                    if(soma_status == 4 && status_op[4] == 1) {
                        printf("\nAPAGAO IMINENTE. DESATIVANDO SISTEMAS NAO VITAIS E ENVIANDO SATELITE DE EMERGENCIA COM PEDIDO DE SOCORRO.\n");
                    }
                }
                break;

            case 0:
                printf("Encerrando sistema...\n");
                break;

            default:
                printf("\nOpcao invalida!\n");
        }

    } while (opcao != 0);

    return 0;
}