import plotly.graph_objs as go
import plotly.io as pio
import os

def load_data(filename):
    with open(filename, 'r') as f:
        content = f.read()

    # Separar por espacios y convertir a enteros
    mensajes = list(map(int, content.strip().split()))
    ciclos = list(range(len(mensajes)))  # Cada índice representa un ciclo
    return ciclos, mensajes

def plot_mensajes_vs_ciclos(ciclos, mensajes, tipo_grafica):
    fig = go.Figure()
    fig.add_trace(go.Scatter(
        x=ciclos,
        y=mensajes,
        mode='lines+markers',
        name= tipo_grafica + ' por Ciclo'
    ))
    fig.update_layout(
        title=tipo_grafica + ' por Ciclo',
        xaxis_title='Ciclos',
        yaxis_title=tipo_grafica,
        template='plotly_white'
    )
    fig.show()  # Abre automáticamente en navegador

if __name__ == '__main__':
    filename = 'interconnect_data_PEs.txt'  # Asegúrate de guardar los datos ahí
    ciclos, mensajes = load_data(filename)
    plot_mensajes_vs_ciclos(ciclos, mensajes)

