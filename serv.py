'''
Сервер путеводителя по Китаю
'''

import os
import getpass
import numpy as np
import pandas as pd
import json

import asyncio
import uvicorn
import logging

from langchain_chroma import Chroma
import chromadb
from chromadb.utils import embedding_functions
from langchain_huggingface.embeddings import HuggingFaceEmbeddings

from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
from fastapi.responses import StreamingResponse

from langchain_gigachat.chat_models import GigaChat
from langchain_core.messages import SystemMessage, HumanMessage, AIMessage, trim_messages

# Инициализация логирования
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Запрашиваем токен
def set_env(var: str):
    if not os.environ.get(var):
        os.environ[var] = getpass.getpass(f"{var}: ")

set_env("GIGACHAT_API_KEY")

# Загружаем модель
model = GigaChat(
    credentials=os.environ.get("GIGACHAT_API_KEY"),
    scope="GIGACHAT_API_PERS",
    model="GigaChat",
    # Отключает проверку наличия сертификатов НУЦ Минцифры
    verify_ssl_certs=False,
    streaming=True, #False,
)

# Загружаем датасет
data = pd.read_csv("2024_12_14 China full 02.csv")

# Грузим эмбеддинги и базу Chroma
sentence_transformer_ef = embedding_functions.SentenceTransformerEmbeddingFunction(model_name="intfloat/multilingual-e5-large")
sentence_transformer_ef_cl = HuggingFaceEmbeddings(model_name="intfloat/multilingual-e5-large")

persistent_client_serv = chromadb.PersistentClient(path="./serv_db_res_crop")
collection_serv = persistent_client_serv.get_or_create_collection("serv_collection", embedding_function=sentence_transformer_ef)

# Создаеём клиента векторной базы
vector_store_from_client = Chroma(
    client=persistent_client_serv,
    collection_name="serv_collection",
    embedding_function=sentence_transformer_ef_cl,
)

# Функция добавления контекста к пользовательскому запросу
def get_human_message(answer: str):
    results = vector_store_from_client.similarity_search(
        answer,
        k=2,
    )
    
    prompt = f"{answer}\nКонтекст:\n"
    if (results[0].metadata['Chapters'] == results[1].metadata['Chapters']) and (results[0].metadata['Subchapters'] == results[1].metadata['Subchapters']):
        prompt += data.loc[(data['Chapters'] == results[0].metadata['Chapters']) & (data['Subchapters'] == results[0].metadata['Subchapters']), 'Clean_Text'].values[0] + "\n"
    else:
        prompt += "* " + data.loc[(data['Chapters'] == results[0].metadata['Chapters']) & (data['Subchapters'] == results[0].metadata['Subchapters']), 'Clean_Text'].values[0] + "\n* " + \
            data.loc[(data['Chapters'] == results[1].metadata['Chapters']) & (data['Subchapters'] == results[1].metadata['Subchapters']), 'Clean_Text'].values[0] + "\n"
    
    ind = int(data.loc[(data['Chapters'] == results[0].metadata['Chapters']) & (data['Subchapters'] == results[0].metadata['Subchapters'])].index[0])

    return (HumanMessage(content=prompt), ind)

# Создаём и инициализируем очередь сообщений
messages = []

messages.append(SystemMessage(content="Ты профессиональный путеводетель по Китаю. Ты подробно отвечаешь на запросы пользователя используя контекст. Придерживайся фактов!"))

# Функция генерации ответов
async def generate(messages, index: int):
    logger.info(f"Запрос пользователя. Index: {index} Запрос: {messages[-1]}")

    index_pack = {
        "index": index
    }
    yield json.dumps(index_pack)
    await asyncio.sleep(0.1)

    ansver = ""
    for chunk in model.stream(messages):
        ansver += chunk.content
        print(chunk.content, end="", flush=True)
        response_part = {
            "content": f"{chunk.content}"
        }

        yield json.dumps(response_part)    # json.dumps(delta)
        await asyncio.sleep(0.025)
    
    messages.append(AIMessage(ansver))
    logger.info(f"Ответ путеводителя. Index: {index} Запрос: {messages[-1]}")

# Инициализация FastAPI
app = FastAPI()

class ChatMessage(BaseModel):
    content: str

@app.post("/chat")
async def chat_endpoint(chat_message: ChatMessage):

    message, index = get_human_message(chat_message.content)
    messages.append(message)
    
    trim_messages(
        messages,
        max_tokens=26000,
        strategy="last",
        token_counter=model,
        include_system=True,
        allow_partial=False,
        start_on="human",
    )

    return StreamingResponse(generate(messages, index), media_type="text/plain")  # return answer

if __name__ == "__main__":
    uvicorn.run(app, host="192.168.0.19", port=9090) 
