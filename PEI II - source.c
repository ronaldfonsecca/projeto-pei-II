#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define MAX_PRODUTOS_RECEBIDOS 20
#define MAX_NOME 50

// Limites de peso para o turno da manhã para os produtos recebidos
#define LIMITE_PESO_MANHA_BASE 10000.0f
#define LIMITE_PESO_MANHA_TETO 11000.0f

typedef struct {
    char nome[MAX_NOME];
    float peso_unitario_kg;
} ProdutoCatalogo;

// Catálogo de produtos disponíveis (ORDENADO PELO USUÁRIO - mais pesado para mais leve)
ProdutoCatalogo catalogo_produtos[] = {
    {"Mussarela", 16.0f},
    {"Mussarela Light", 16.0f},
    {"Lanchão", 12.0f},
    {"Parmesão", 9.0f},
    {"Frescal Grande", 9.0f},
    {"Cheddar", 7.0f},
    {"Lanchinho", 6.0f},
    {"Minas Padrão", 6.0f},
    {"Minas Fracionado", 6.0f},
    {"Minas Fracionado Zero Lactose", 6.0f},
    {"Ricota", 6.0f},
    {"Frescal Pequeno", 6.0f},
    {"Mussarelinha", 5.0f},
    {"Requeijão Tablete", 4.0f},
    {"Coalho", 4.0f},
    {"Parmesão Fatiado", 4.0f},
    {"Reino", 2.0f}
};
int num_catalogo_produtos = sizeof(catalogo_produtos) / sizeof(catalogo_produtos[0]);

typedef struct {
    char nome[MAX_NOME];
    int caixas;
    float peso_unitario;
    float peso_total;
    int turno;
} ProdutoRecebido;

ProdutoRecebido produtos_recebidos[MAX_PRODUTOS_RECEBIDOS];
int total_produtos_recebidos = 0;

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void calcularPesoTotalRecebido(ProdutoRecebido* p) {
    p->peso_total = p->caixas * p->peso_unitario;
}

void ordenarProdutosRecebidosPorPeso() {
    ProdutoRecebido temp;
    for (int i = 0; i < total_produtos_recebidos - 1; i++) {
        for (int j = 0; j < total_produtos_recebidos - i - 1; j++) {
            if (produtos_recebidos[j].peso_total < produtos_recebidos[j + 1].peso_total) {
                temp = produtos_recebidos[j];
                produtos_recebidos[j] = produtos_recebidos[j + 1];
                produtos_recebidos[j + 1] = temp;
            }
        }
    }
}

void distribuirTurnos() {
    if (total_produtos_recebidos == 0) {
        return;
    }
    ordenarProdutosRecebidosPorPeso();
    float peso_acumulado_manha = 0.0f;

    // DEBUG
    // printf("\nDebug (distribuirTurnos): --- Iniciando distribuição para %d produto(s) ---\n", total_produtos_recebidos);
    // printf("Debug (distribuirTurnos): Limite Manhã Base: %.2f kg, Teto: %.2f kg\n", LIMITE_PESO_MANHA_BASE, LIMITE_PESO_MANHA_TETO);

    for (int i = 0; i < total_produtos_recebidos; i++) {
        float peso_produto_atual = produtos_recebidos[i].peso_total;
        // printf("\nDebug (distribuirTurnos): Produto %d: '%s' (Peso: %.2f kg)\n", i + 1, produtos_recebidos[i].nome, peso_produto_atual);
        // printf("Debug (distribuirTurnos): Acumulado Manhã ANTES: %.2f kg. Com este: %.2f kg\n", peso_acumulado_manha, peso_acumulado_manha + peso_produto_atual);

        if ((peso_acumulado_manha + peso_produto_atual) <= LIMITE_PESO_MANHA_BASE) {
            produtos_recebidos[i].turno = 1; // Manhã
            peso_acumulado_manha += peso_produto_atual;
            // printf("Debug (distribuirTurnos): -> CASO 1: '%s' MANHÃ. Acumulado Manhã: %.2f kg\n", produtos_recebidos[i].nome, peso_acumulado_manha);
        } else if (peso_acumulado_manha < LIMITE_PESO_MANHA_BASE &&
                   (peso_acumulado_manha + peso_produto_atual) <= LIMITE_PESO_MANHA_TETO) {
            produtos_recebidos[i].turno = 1; // Manhã (com tolerância)
            peso_acumulado_manha += peso_produto_atual;
            // printf("Debug (distribuirTurnos): -> CASO 2 (TOLERÂNCIA): '%s' MANHÃ. Acumulado Manhã: %.2f kg. Próximos para TARDE.\n", produtos_recebidos[i].nome, peso_acumulado_manha);
            for (int j = i + 1; j < total_produtos_recebidos; j++) {
                produtos_recebidos[j].turno = 2; // Tarde
                // printf("Debug (distribuirTurnos):    -> '%s' TARDE.\n", produtos_recebidos[j].nome);
            }
            // printf("Debug (distribuirTurnos): --- Fim (Caso 2) ---\n");
            return;
        } else {
            // printf("Debug (distribuirTurnos): -> CASO 3: '%s' NÃO CABE na manhã. Indo para TARDE.\n", produtos_recebidos[i].nome);
            for (int j = i; j < total_produtos_recebidos; j++) {
                produtos_recebidos[j].turno = 2; // Tarde
                // if (j != i) printf("Debug (distribuirTurnos):    -> '%s' TARDE.\n", produtos_recebidos[j].nome);
            }
            // printf("Debug (distribuirTurnos): --- Fim (Caso 3) ---\n");
            return;
        }
    }
    // printf("Debug (distribuirTurnos): --- Fim (Todos processados) ---\n");
}

void exibirProdutoRecebido(ProdutoRecebido p, int indice_lista) {
    char turno_str[10];
    if (p.turno == 1) strcpy(turno_str, "Manhã");
    else if (p.turno == 2) strcpy(turno_str, "Tarde");
    else strcpy(turno_str, "N/D"); // Não Definido
    printf("%dº - %s | Caixas: %d | Peso Unitário: %.2f kg | Peso Total: %.2f kg | Turno: %s\n",
           indice_lista + 1, p.nome, p.caixas, p.peso_unitario, p.peso_total, turno_str);
}

void listarProdutosRecebidos() {
    if (total_produtos_recebidos == 0) {
        printf("Nenhum produto recebido cadastrado ainda.\n\n");
        return;
    }
    printf("\n--- Lista de Produtos Recebidos (ordenados do mais pesado para o mais leve em peso total) ---\n");
    for (int i = 0; i < total_produtos_recebidos; i++) {
        exibirProdutoRecebido(produtos_recebidos[i], i);
    }

    float peso_total_manha_calc = 0.0f, peso_total_tarde_calc = 0.0f;
    int count_manha = 0, count_tarde = 0;
    for (int i = 0; i < total_produtos_recebidos; i++) {
        if (produtos_recebidos[i].turno == 1) {
            peso_total_manha_calc += produtos_recebidos[i].peso_total;
            count_manha++;
        } else if (produtos_recebidos[i].turno == 2) {
            peso_total_tarde_calc += produtos_recebidos[i].peso_total;
            count_tarde++;
        }
    }
    printf("\n--- Resumo dos Turnos dos Produtos Recebidos ---\n");
    printf("Turno Manhã: %d produto(s), Peso Total Acumulado: %.2f kg (Limite Base: %.0f kg, Teto: %.0f kg)\n",
           count_manha, peso_total_manha_calc, LIMITE_PESO_MANHA_BASE, LIMITE_PESO_MANHA_TETO);
    printf("Turno Tarde: %d produto(s), Peso Total Acumulado: %.2f kg\n\n", count_tarde, peso_total_tarde_calc);
}

void exibirCatalogoParaSelecao() {
    printf("\n--- Produtos Disponíveis no Catálogo (ordem pré-definida) ---\n");
    for (int i = 0; i < num_catalogo_produtos; i++) {
        printf(" [%2d] %-30s (%.2f kg/unidade)\n", i + 1, catalogo_produtos[i].nome, catalogo_produtos[i].peso_unitario_kg);
    }
    printf(" [ 0] Cancelar Cadastro\n");
}

void cadastrarRecebimentoProduto() {
    if (total_produtos_recebidos >= MAX_PRODUTOS_RECEBIDOS) {
        printf("Limite máximo de %d tipos de produtos recebidos atingido para este dia.\n\n", MAX_PRODUTOS_RECEBIDOS);
        return;
    }
    exibirCatalogoParaSelecao();
    int escolha_catalogo;
    printf("\nEscolha o NÚMERO do produto a ser cadastrado (ou 0 para cancelar): ");
    while (scanf("%d", &escolha_catalogo) != 1 || escolha_catalogo < 0 || escolha_catalogo > num_catalogo_produtos) {
        printf("Escolha inválida. Digite um número da lista (1 a %d, ou 0 para cancelar): ", num_catalogo_produtos);
        limparBuffer();
    }
    if (escolha_catalogo == 0) {
        printf("Cadastro de recebimento cancelado.\n\n");
        return;
    }
    int indice_catalogo = escolha_catalogo - 1;
    ProdutoRecebido pr;
    pr.turno = 0;
    strcpy(pr.nome, catalogo_produtos[indice_catalogo].nome);
    pr.peso_unitario = catalogo_produtos[indice_catalogo].peso_unitario_kg;
    printf("Produto selecionado: %s (%.2f kg/unidade)\n", pr.nome, pr.peso_unitario);
    printf("Informe a quantidade de caixas/unidades recebidas para '%s': ", pr.nome);
    while (scanf("%d", &pr.caixas) != 1 || pr.caixas <= 0) {
        printf("Entrada inválida. Digite um número inteiro positivo para a quantidade: ");
        limparBuffer();
    }
    calcularPesoTotalRecebido(&pr);
    produtos_recebidos[total_produtos_recebidos] = pr;
    total_produtos_recebidos++;
    distribuirTurnos();
    printf("\nRecebimento de '%s' (Quantidade: %d) cadastrado com sucesso!\n\n", pr.nome, pr.caixas);
    listarProdutosRecebidos();
}

void consultarProdutosRecebidosPorTurno() {
    if (total_produtos_recebidos == 0) {
        printf("Nenhum produto recebido cadastrado para consultar.\n\n");
        return;
    }
    int opcao_turno;
    printf("\n--- Consultar Produtos Recebidos por Turno ---\n");
    printf("Qual turno deseja consultar? [1] Manhã | [2] Tarde: ");
    while (scanf("%d", &opcao_turno) != 1 || (opcao_turno != 1 && opcao_turno != 2)) {
        printf("Opção inválida. Digite 1 para Manhã ou 2 para Tarde: ");
        limparBuffer();
    }
    char *nome_turno_consulta = (opcao_turno == 1) ? "Manhã" : "Tarde";
    printf("\n--- Produtos Recebidos no turno %s ---\n", nome_turno_consulta);
    int encontrados = 0;
    float peso_acumulado_consulta = 0.0f;
    for (int i = 0; i < total_produtos_recebidos; i++) {
        if (produtos_recebidos[i].turno == opcao_turno) {
            exibirProdutoRecebido(produtos_recebidos[i], encontrados);
            peso_acumulado_consulta += produtos_recebidos[i].peso_total;
            encontrados++;
        }
    }
    if (encontrados == 0) printf("Nenhum produto recebido encontrado no turno %s.\n", nome_turno_consulta);
    else {
        printf("\nTotal de produtos recebidos no turno %s: %d\n", nome_turno_consulta, encontrados);
        printf("Peso total acumulado no turno %s: %.2f kg\n", nome_turno_consulta, peso_acumulado_consulta);
    }
    printf("\n");
}

void deletarProdutoRecebido() {
    if (total_produtos_recebidos == 0) {
        printf("Nenhum produto recebido para deletar.\n\n");
        return;
    }
    printf("\n--- Deletar Produto Recebido ---\n");
    listarProdutosRecebidos();
    int numero_produto_para_deletar;
    printf("Digite o NÚMERO do produto recebido que deseja deletar (1 a %d): ", total_produtos_recebidos);
    while (scanf("%d", &numero_produto_para_deletar) != 1 || numero_produto_para_deletar < 1 || numero_produto_para_deletar > total_produtos_recebidos) {
        printf("Número inválido. Digite um número entre 1 e %d: ", total_produtos_recebidos);
        limparBuffer();
    }
    int indice_array = numero_produto_para_deletar - 1;
    char nome_deletado[MAX_NOME];
    strcpy(nome_deletado, produtos_recebidos[indice_array].nome);
    printf("Produto Recebido \"%s\" selecionado para deleção.\n", nome_deletado);
    for (int i = indice_array; i < total_produtos_recebidos - 1; i++) {
        produtos_recebidos[i] = produtos_recebidos[i + 1];
    }
    total_produtos_recebidos--;
    if (total_produtos_recebidos > 0) distribuirTurnos();
    else printf("\nTodos os produtos recebidos foram deletados.\n");
    printf("Produto Recebido \"%s\" deletado com sucesso.\n\n", nome_deletado);
    if (total_produtos_recebidos > 0) {
        printf("Lista de produtos recebidos atualizada:\n");
        listarProdutosRecebidos();
    }
}

int main() {
    setlocale(LC_ALL, "Portuguese");
    int opcao;
    printf("Bem-vindo ao Sistema de Organização de Pesagem - Selita Laticínios (Simulação)\n");
    do {
        printf("\n          ===== MENU PRINCIPAL =====\n");
        printf(" [1] Cadastrar recebimento de produto do catálogo\n");
        printf(" [2] Consultar todos os produtos recebidos (ordenados por peso total)\n");
        printf(" [3] Consultar produtos recebidos por turno\n");
        printf(" [4] Deletar um produto recebido\n");
        printf(" [0] Sair do sistema\n\n");
        printf("Escolha uma opção: ");
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida. Por favor, digite um número.\n");
            limparBuffer();
            opcao = -1;
        }
        switch (opcao) {
            case 1: cadastrarRecebimentoProduto(); break;
            case 2: listarProdutosRecebidos(); break;
            case 3: consultarProdutosRecebidosPorTurno(); break;
            case 4: deletarProdutoRecebido(); break;
            case 0: printf("\nEncerrando o sistema... Até logo!\n"); break;
            default: if (opcao != -1) printf("Opção inválida. Tente novamente.\n\n");
        }
    } while (opcao != 0);
    return 0;
}
