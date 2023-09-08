import paho.mqtt.subscribe as subscribe


def on_message_print(client, userdata, message):
    print(f"{client} {userdata} {message.topic} {message.payload}")


if __name__ == "__main__":
    subscribe.callback(on_message_print, "#", hostname="66.253.158.154")
