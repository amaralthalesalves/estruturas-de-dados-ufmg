#ifndef TIPOS_H
#define TIPOS_H

// Par que aparece tanto no comando C quanto no comando R
struct ItemEstoque {
    int idProduto;
    int quantidade;
};

struct ProdutoComprado {
    int idProduto;
    int qtdComprada;
};

// Mesmo agregado do outro lado da relacao, usado nas consultas de produto
struct UsuarioComprador {
    int idUsuario;
    int qtdComprada;
};

#endif
