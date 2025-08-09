# saludo_client.py
import grpc
import saludo_pb2
import saludo_pb2_grpc

def run():
    channel = grpc.insecure_channel('localhost:50051')
    stub = saludo_pb2_grpc.SaludoServiceStub(channel)

    nombre = input("¿Cómo te llamas? ")
    respuesta = stub.Saludar(saludo_pb2.SaludoRequest(nombre=nombre))
    print("Respuesta del servidor:", respuesta.mensaje)

if __name__ == "__main__":
    run()
