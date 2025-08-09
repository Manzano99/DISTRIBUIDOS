import grpc
import calculadora_pb2
import calculadora_pb2_grpc

def run():
    channel = grpc.insecure_channel('localhost:50052')
    stub = calculadora_pb2_grpc.CalculadoraServiceStub(channel)

    while True:
        print("----- Calculadora -----")
        print("1. Sumar")
        print("2. Restar")
        print("3. Multiplicar")
        print("4. Dividir")
        print("5. Ver historial")
        print("6. Limpiar historial")
        print("0. Salir")
        opcion = input("Selecciona una opción: ")

        if opcion == "0":
            break
        elif opcion == "5":
            print("\n--- Historial de operaciones ---")
            respuesta_stream = stub.Historial(calculadora_pb2.HistorialRequest())
            for respuesta in respuesta_stream:
                print(respuesta.descripcion)
            print("------------------------------\n")
        elif opcion == "6":
            respuesta = stub.LimpiarHistorial(calculadora_pb2.HistorialRequest())
            print(respuesta.mensaje)
            print("------------------------------\n")
        elif opcion in ["1", "2", "3", "4"]:
            try:
                n1 = float(input("Introduce el primer número: "))
                n2 = float(input("Introduce el segundo número: "))
            except ValueError:
                print("Entrada no válida. Intenta de nuevo.")
                continue

            request = calculadora_pb2.OperacionRequest(numero1=n1, numero2=n2)

            if opcion == "1":
                respuesta = stub.Sumar(request)
            elif opcion == "2":
                respuesta = stub.Restar(request)
            elif opcion == "3":
                respuesta = stub.Multiplicar(request)
            elif opcion == "4":
                respuesta = stub.Dividir(request)

            if respuesta.mensaje:
                print(respuesta.mensaje)
            elif respuesta.resultado:
                print("Resultado:", respuesta.resultado)
        else:
            print("Opción no válida.")

if __name__ == "__main__":
    run()
