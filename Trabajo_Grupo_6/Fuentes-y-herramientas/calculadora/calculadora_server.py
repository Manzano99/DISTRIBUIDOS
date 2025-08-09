import grpc
from concurrent import futures
import time
import calculadora_pb2
import calculadora_pb2_grpc

class Calculadora(calculadora_pb2_grpc.CalculadoraServiceServicer):
    def __init__(self):
        self.historial = []

    def Sumar(self, request, context):
        resultado = request.numero1 + request.numero2
        desc = f"{request.numero1} + {request.numero2} = {resultado}"
        self.historial.append(desc)
        return calculadora_pb2.OperacionReply(resultado=resultado)

    def Restar(self, request, context):
        resultado = request.numero1 - request.numero2
        desc = f"{request.numero1} - {request.numero2} = {resultado}"
        self.historial.append(desc)
        return calculadora_pb2.OperacionReply(resultado=resultado)

    def Multiplicar(self, request, context):
        resultado = request.numero1 * request.numero2
        desc = f"{request.numero1} * {request.numero2} = {resultado}"
        self.historial.append(desc)
        return calculadora_pb2.OperacionReply(resultado=resultado)

    def Dividir(self, request, context):
        if request.numero2 == 0:
            return calculadora_pb2.OperacionReply(resultado=0, mensaje="Error: División por cero")
        else:
            resultado = request.numero1 / request.numero2
            desc = f"{request.numero1} / {request.numero2} = {resultado}"
            self.historial.append(desc)
            return calculadora_pb2.OperacionReply(resultado=resultado)

    def Historial(self, request, context):
        for operacion in self.historial:
            yield calculadora_pb2.HistorialReply(descripcion=operacion)

    def LimpiarHistorial(self, request, context):
        self.historial.clear()
        return calculadora_pb2.LimpiarReply(mensaje="Historial limpiado correctamente.")

def servir():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    calculadora_pb2_grpc.add_CalculadoraServiceServicer_to_server(Calculadora(), server)
    server.add_insecure_port('[::]:50052')
    server.start()
    print("Servidor gRPC de Calculadora en ejecución en puerto 50052...")
    server.wait_for_termination()

if __name__ == '__main__':
    servir()
