# 🚀 Guia Rápido - Subir Projeto no GitHub

Este guia mostra como versionar seu Gateway LoRa P2P no GitHub passo a passo.

---

## 📋 Pré-requisitos

- Git instalado no seu sistema
- Conta no GitHub
- Projeto na pasta local

---

## 🔧 Passo 1: Criar Repositório no GitHub

1. Acesse: https://github.com/new
2. Preencha:
   - **Repository name:** `gateway-lora-p2p` (ou nome de sua escolha)
   - **Description:** `Gateway LoRa P2P single channel com ESP32 e SX1276`
   - **Visibility:** Público ou Privado
3. **NÃO** marque nenhuma opção (README, .gitignore, LICENSE)
4. Clique em **Create repository**

---

## 💻 Passo 2: Preparar o Projeto Localmente

Abra o terminal na pasta do seu projeto:

```bash
# Navegar até a pasta (exemplo)
cd /caminho/para/Gateway_LoRaPtP_Fixo_v1_3

# Inicializar repositório Git
git init

# Configurar seu nome e email (se ainda não fez)
git config --global user.name "Seu Nome"
git config --global user.email "seu@email.com"
```

---

## 📁 Passo 3: Organizar a Estrutura

Certifique-se que a estrutura está assim:

```
Gateway_LoRaPtP_Fixo_v1_3/
├── .gitignore                          ← Adicionar
├── LICENSE                             ← Adicionar
├── README.md                           ← Adicionar
├── platformio.ini                      ← Opcional
├── Gateway_LoRaPtP_Fixo_v1_3.ino
├── config.h
├── protocol.h
├── lora_p2p.h
├── lora_p2p.cpp
├── sensors.h
├── sensors.cpp
├── display_oled.h
├── display_oled.cpp
├── wifi_mqtt.h
├── wifi_mqtt.cpp
├── at_commands.h
└── at_commands.cpp
```

**Copie os arquivos que criei:**
- `README.md` (substitua o seu se já tiver um)
- `.gitignore`
- `LICENSE`
- `platformio.ini` (opcional)

---

## ✅ Passo 4: Fazer o Primeiro Commit

```bash
# Adicionar todos os arquivos
git add .

# Verificar o que será commitado
git status

# Fazer o commit inicial
git commit -m "Initial commit: Gateway LoRa P2P v1.3"
```

---

## 🔗 Passo 5: Conectar ao GitHub

**Substitua `SEU_USUARIO` e `NOME_REPO` pelos seus dados:**

```bash
# Adicionar repositório remoto
git remote add origin https://github.com/SEU_USUARIO/NOME_REPO.git

# Renomear branch para main (se necessário)
git branch -M main

# Enviar para o GitHub
git push -u origin main
```

---

## 🔑 Passo 6: Autenticação

### Opção A: Token de Acesso Pessoal (Recomendado)

1. GitHub → **Settings** → **Developer settings** → **Personal access tokens** → **Tokens (classic)**
2. Clique em **Generate new token (classic)**
3. Dê um nome: `Git Gateway LoRa`
4. Marque: `repo` (acesso completo ao repositório)
5. Clique em **Generate token**
6. **COPIE O TOKEN** (não será mostrado novamente!)
7. Quando o git pedir senha, **cole o token**

### Opção B: SSH (Alternativa)

```bash
# Gerar chave SSH (se não tiver)
ssh-keygen -t ed25519 -C "seu@email.com"

# Copiar a chave pública
cat ~/.ssh/id_ed25519.pub

# Adicionar no GitHub:
# GitHub → Settings → SSH and GPG keys → New SSH key

# Mudar URL do remote para SSH
git remote set-url origin git@github.com:SEU_USUARIO/NOME_REPO.git

# Testar conexão
ssh -T git@github.com
```

---

## 📝 Passo 7: Verificar no GitHub

Acesse: `https://github.com/SEU_USUARIO/NOME_REPO`

Você deve ver:
- ✅ README.md formatado
- ✅ Todos os arquivos do projeto
- ✅ Licença MIT
- ✅ .gitignore configurado

---

## 🔄 Comandos Úteis para Futuras Atualizações

### Atualizar código após mudanças

```bash
# Ver status dos arquivos
git status

# Adicionar arquivos modificados
git add .

# Ou adicionar arquivo específico
git add lora_p2p.cpp

# Commit com mensagem descritiva
git commit -m "Fix: Corrige timeout de reconexão WiFi"

# Enviar para GitHub
git push
```

### Ver histórico de commits

```bash
git log --oneline --graph --all
```

### Criar uma branch para nova feature

```bash
# Criar e mudar para nova branch
git checkout -b feature/mqtt-buffer-increase

# Fazer mudanças, adicionar e commitar
git add .
git commit -m "Increase MQTT buffer to 1024 bytes"

# Enviar branch para GitHub
git push -u origin feature/mqtt-buffer-increase
```

### Desfazer mudanças não commitadas

```bash
# Descartar mudanças em arquivo específico
git checkout -- arquivo.cpp

# Descartar TODAS as mudanças
git reset --hard
```

---

## 📊 Badges para o README (Opcional)

Adicione badges no topo do README.md:

```markdown
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![Version](https://img.shields.io/badge/version-1.3-blue.svg)]()
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-ESP32-orange.svg)]()
[![LoRa](https://img.shields.io/badge/LoRa-915MHz-red.svg)]()
```

---

## 🎯 Checklist Final

Antes de compartilhar o repositório:

- [ ] README.md está completo e formatado
- [ ] .gitignore exclui arquivos desnecessários
- [ ] LICENSE está presente
- [ ] Código está comentado adequadamente
- [ ] Pinos e configurações estão documentados
- [ ] Exemplos de uso estão claros
- [ ] Links funcionam (se tiver)
- [ ] Imagens/diagramas estão no repositório (se usar)

---

## 🆘 Problemas Comuns

### "fatal: remote origin already exists"

```bash
git remote remove origin
git remote add origin https://github.com/SEU_USUARIO/NOME_REPO.git
```

### "Updates were rejected"

```bash
# Forçar push (CUIDADO: sobrescreve histórico remoto)
git push -f origin main
```

### "Permission denied (publickey)"

Configure SSH corretamente ou use HTTPS com token.

---

## 📚 Recursos

- [GitHub Docs](https://docs.github.com/)
- [Git Cheat Sheet](https://education.github.com/git-cheat-sheet-education.pdf)
- [Markdown Guide](https://www.markdownguide.org/)

---

## ✅ Pronto!

Seu projeto agora está versionado e disponível no GitHub! 🎉

**Próximos passos:**
1. Compartilhe o link com a comunidade
2. Adicione issues/features que planeja implementar
3. Aceite contribuições da comunidade
4. Mantenha o README atualizado

**Link do seu projeto:** `https://github.com/SEU_USUARIO/NOME_REPO`
