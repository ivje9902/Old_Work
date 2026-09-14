from google.genai.types import GenerateContentConfig
from pydantic import BaseModel, Field

# Define a prompt that helps with DST
system_prompt = "You are a friendly robot interacting with a user based on a fixed script. For every message of the user you will also be given a topic to talk about and the next topic to ask next after your short reply. Keep replies short and conversational."

class ResponseWithState(BaseModel):
    reply: str = Field(description="Normal reply to the user message")
    next_state: bool = Field(description="True if the conversation has wrapped up and is ready to move to the next topic")

# pass this config when creating a chat
config = GenerateContentConfig(
    system_instruction=system_prompt,
    response_mime_type="application/json",
    response_json_schema=ResponseWithState.model_json_schema()
)

# Define some topics you want to talk about
topic_list = [
    "Ask the user how their day was.",
    "Ask the user how they are doing.",
    "Ask them what is on their mind.",
    "Ask the user to rate their stress level on a scale of 1-10 ",
    "Ask about the main source of the stress.",
    "Ask how they cope with stress.",
    "Ask if they had any positive moments today.",
    "Thank them and say goodbye."
]
