# ⚡ cli-chat-server

> Um servidor de chat TCP minimalista, direto no terminal — rápido, leve e sem frescura.

---

## 🎯 Setup rápido

### Ajuste IP & PORT no `Makefile`

Configure o servidor apontando para seu IP e porta preferida:

```makefile
IP = 192.168.0.103   # Seu IP aqui, pode ser local ou público
PORT = 8996          # Porta que o servidor vai escutar
```

> 🚨 Dica: Use o IP da máquina que vai rodar o servidor para conectar de outros dispositivos.

---

## 🚀 Build + Run em 1 comando

Compile e já rode o servidor na sequência:

```bash
make server && make run
```

> Vai aparecer algo assim:

```bash
[16:29:08] 👂 Escutando em 192.168.0.103:8996
```

---

## 🤝 Conecte-se sem complicação

Sem cliente pesado, sem setup extra — só abra o terminal e use qualquer ferramenta TCP:

* `netcat` (`nc`)
* `telnet`
* Qualquer outro cliente TCP básico

### Exemplo:

```bash
nc 192.168.0.103 8996
```

> Lembre de trocar pelo IP do seu servidor!

---

## 💬 Bate-papo & comandos

### `/nick <nome>`

Defina seu apelido para aparecer na conversa.

```bash
/nick Ana
```

Sem apelido? Você vira **@Desconhecido**.

---

### Exemplo de conversa

```plaintext
Ana: Fala, galera!
@Desconhecido: Oi Ana!
```
---