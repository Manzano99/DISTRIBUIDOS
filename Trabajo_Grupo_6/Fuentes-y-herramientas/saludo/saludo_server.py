# saludo_server.py
import grpc
from concurrent import futures
import saludo_pb2
import saludo_pb2_grpc

class SaludoService(saludo_pb2_grpc.SaludoServiceServicer):
    def Saludar(self, request, context):
        mensaje = f"Hola, {request.nombre}!"
        return saludo_pb2.SaludoReply(mensaje=mensaje)

def servir():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=5))
    saludo_pb2_grpc.add_SaludoServiceServicer_to_server(SaludoService(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    print("Servidor gRPC en ejecución en el puerto 50051...")
    server.wait_for_termination()

if __name__ == "__main__":
    servir()
