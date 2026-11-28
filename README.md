# MyHealth — Amostra das Tabelas de Dados (PostgreSQL)

Banco inspecionado no container `postgres` (`postgres:17-alpine`, database `myhealth`, schema `public`).

## Tabelas existentes no banco

| Tabela | Schema | Tipo | Tamanho | Registros |
| --- | --- | --- | --- | --- |
| `readings` | `public` | table | 264 kB | 2.119 |
| `flyway_schema_history` | `public` | table | 16 kB | 1 |

> **Observação:** Existe a entidade JPA [`Device`](api/src/main/java/com/myhealth/api/models/Device.java) na API, mas ainda não há tabela `device` / `devices` criada no banco.

---

## 1. Tabela `public.readings`

Armazena as leituras enviadas via MQTT e inseridas pelo Bento (`bento/bento.yaml`).

### Estrutura das colunas

| Coluna | Tipo | Nulo | Default | Observação |
| --- | --- | --- | --- | --- |
| `id` | `uuid` | `NOT NULL` | `gen_random_uuid()` | Chave primária (`readings_pkey`) |
| `time` | `timestamp with time zone` | `NULL` (no schema) / `NOT NULL` no init do Bento | — | Timestamp da leitura |
| `device_id` | `uuid` | `NULL` (no schema) / `NOT NULL` no init do Bento | — | Identificador do dispositivo |
| `state` | `text` | `NULL` (no schema) / `NOT NULL` no init do Bento | — | Estado do sensor (`IDLE`, `MEASURING`, `CALIBRATING`, `WEAK_SIGNAL`) |
| `bpm` | `double precision` | `NULL` (no schema) / `NOT NULL` no init do Bento | — | Batimentos por minuto |

### Distribuição atual por `state` (1 dispositivo distinto: `01a160c0-7517-745b-8241-e7f8d680449f`)

| `state` | Quantidade | `min_bpm` | `max_bpm` | `avg_bpm` | Primeira leitura | Última leitura |
| --- | ---: | ---: | ---: | ---: | --- | --- |
| `IDLE` | 2.104 | 0 | 0 | 0.00 | `1970-01-01 00:00:05+00` | `2026-11-25 01:25:33+00` |
| `MEASURING` | 10 | 75.8 | 95.9 | 86.55 | `2026-11-23 23:15:10+00` | `2026-11-23 23:15:16+00` |
| `CALIBRATING` | 3 | 0 | 0 | 0.00 | `2026-11-23 23:14:39+00` | `2026-11-25 01:24:22+00` |
| `WEAK_SIGNAL` | 2 | 0 | 0 | 0.00 | `2026-11-23 23:14:50+00` | `2026-11-23 23:15:03+00` |

### Amostra de registros (`public.readings`)

| `id` | `time` | `device_id` | `state` | `bpm` |
| --- | --- | --- | --- | ---: |
| `cf27dbd1-d2fe-4029-9115-c6ad1c700c4f` | `2026-11-23 23:15:16+00` | `01a160c0-7517-745b-8241-e7f8d680449f` | `MEASURING` | `95.9` |
| `fd9dbd6c-2b47-4332-a419-71feaf24bb1e` | `2026-11-23 23:15:16+00` | `01a160c0-7517-745b-8241-e7f8d680449f` | `MEASURING` | `92.7` |
| `42f069b5-2090-4340-b4d1-a21d78c06ee9` | `2026-11-23 23:15:15+00` | `01a160c0-7517-745b-8241-e7f8d680449f` | `MEASURING` | `85.2` |
| `8589cf79-9d3b-4c71-8bea-eb24b9f6d658` | `2026-11-23 23:15:15+00` | `01a160c0-7517-745b-8241-e7f8d680449f` | `MEASURING` | `89.6` |
| `be3b1746-ba10-4096-9ed7-91421dc68ada` | `2026-11-23 23:15:14+00` | `01a160c0-7517-745b-8241-e7f8d680449f` | `MEASURING` | `87.8` |
| `514ee896-88d1-4244-8b6a-cea6b978c595` | `2026-11-25 01:24:22+00` | `01a160c0-7517-745b-8241-e7f8d680449f` | `CALIBRATING` | `0` |
| `888239ea-4361-4d65-9dd5-8c897b6199da` | `2026-11-23 23:15:09+00` | `01a160c0-7517-745b-8241-e7f8d680449f` | `CALIBRATING` | `0` |
| `d3c1111d-c3de-48e1-bec6-5a6845a10a99` | `2026-11-23 23:15:03+00` | `01a160c0-7517-745b-8241-e7f8d680449f` | `WEAK_SIGNAL` | `0` |
| `a4e1a083-cc9d-4cca-bcec-296e474b171d` | `2026-11-23 23:14:50+00` | `01a160c0-7517-745b-8241-e7f8d680449f` | `WEAK_SIGNAL` | `0` |
| `ec8ade0d-8590-4bad-8ee9-cf0c5d8b0d20` | `2026-11-25 01:25:33+00` | `01a160c0-7517-745b-8241-e7f8d680449f` | `IDLE` | `0` |
| `44563d82-fa8c-4ce3-af1d-faa83f0f2af4` | `2026-11-25 01:25:28+00` | `01a160c0-7517-745b-8241-e7f8d680449f` | `IDLE` | `0` |
| `de039064-5b9a-4c16-904a-2edc8e2e565a` | `2026-11-25 01:25:23+00` | `01a160c0-7517-745b-8241-e7f8d680449f` | `IDLE` | `0` |

---

## 2. Tabela `public.flyway_schema_history`

Tabela de controle de migrations do Flyway (Spring Boot).

### Estrutura das colunas

| Coluna | Tipo | Nulo | Default | Observação |
| --- | --- | --- | --- | --- |
| `installed_rank` | `integer` | `NOT NULL` | — | Chave primária (`flyway_schema_history_pk`) |
| `version` | `character varying(50)` | `NULL` | — | Versão da migration |
| `description` | `character varying(200)` | `NOT NULL` | — | Descrição |
| `type` | `character varying(20)` | `NOT NULL` | — | Tipo (`BASELINE`, `SQL`, etc.) |
| `script` | `character varying(1000)` | `NOT NULL` | — | Script aplicado |
| `checksum` | `integer` | `NULL` | — | Checksum |
| `installed_by` | `character varying(100)` | `NOT NULL` | — | Usuário do banco |
| `installed_on` | `timestamp without time zone` | `NOT NULL` | `now()` | Data/hora de execução |
| `execution_time` | `integer` | `NOT NULL` | — | Tempo em ms |
| `success` | `boolean` | `NOT NULL` | — | Status (`flyway_schema_history_s_idx`) |

### Registros (`public.flyway_schema_history`)

| `installed_rank` | `version` | `description` | `type` | `script` | `checksum` | `installed_by` | `installed_on` | `execution_time` | `success` |
| ---: | --- | --- | --- | --- | --- | --- | --- | ---: | --- |
| `1` | `0` | `<< Flyway Baseline >>` | `BASELINE` | `<< Flyway Baseline >>` | `NULL` | `myhealth` | `2026-11-24 21:01:19.550136` | `0` | `t` |

---

## Como consultar via Podman

```bash
podman start postgres
podman exec -it postgres psql -U myhealth -d myhealth -c "\dt+" -c "SELECT * FROM public.readings ORDER BY time DESC LIMIT 10;"
```
