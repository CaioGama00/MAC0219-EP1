#!/usr/bin/python3
import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
csv_file = 'stats.csv'
data = pd.read_csv(csv_file)

# Separate the data based on the number of threads
num_threads = sorted(data['num_threads'].unique())

# Plotting the graphs
for i in num_threads:
    # Filter the data for the current number of threads
    filtered_data = data[data['num_threads'] == i]

    # Group the filtered data by 'impl' and 'grid_size' and calculate the average time
    grouped_data = filtered_data.groupby(['impl', 'grid_size'])['time'].mean().reset_index()

    # Plot the lines for each implementation
    for impl in grouped_data['impl'].unique():
        impl_data = grouped_data[grouped_data['impl'] == impl]
        plt.plot(impl_data['grid_size'], impl_data['time'], label=impl)

    # Set the title and labels for the graph
    plt.title(f'Gráfico para num_threads = {i}')
    plt.xlabel('Grid size')
    plt.ylabel('time (s)')
    plt.legend()

    # Save the graph as a PNG file
    plt.savefig(f'plot_{i}.png')

    # Clear the current figure
    plt.clf()