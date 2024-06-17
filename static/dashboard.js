$(document).ready(function() {
    const socket = io.connect('http://127.0.0.1:5000');

    const table = $('#data-table').DataTable({
        order: [[0, 'desc']]
    });

    let currentPage = 0;
    let pageSize = table.page.len(); 

    function updateTable(data) {
        console.log("Updating table with data:", data);
        
        const pageInfo = table.page.info();
        currentPage = pageInfo.page;
        pageSize = pageInfo.length;

        table.clear();
        data.forEach(row => {
            table.row.add([row[0], row[1]]);
        });
        table.draw();

        table.page(currentPage).draw(false);
        table.page.len(pageSize).draw(false);
    }

    socket.on('connect', function() {
        console.log('Connected to WebSocket'); 
    });

    const fuelChart = document.getElementById('fuelChart').getContext('2d');
    let chart = null;

    function updateChart(data) {
        data.reverse();
        
        const labels = data.map(row => moment(row[0]).format('YYYY-MM-DD HH:mm')); // Extract datetime labels
        const levels = data.map(row => row[1]); // Extract level values

        if (chart) {
            chart.data.labels = labels;
            chart.data.datasets[0].data = levels;
            chart.update();
        } else {
            chart = new Chart(fuelChart, {
                type: 'line', // Choose chart type (e.g., line, bar, etc.)
                data: {
                    labels: labels,
                    datasets: [{
                        label: 'Fuel Level',
                        data: levels,
                        borderColor: 'blue',
                        borderWidth: 1,
                        fill: false 
                    }]
                },
                options: {
                    scales: {
                        y: {
                            beginAtZero: true // Start y-axis at 0
                        }
                    }
                }
            });
        }
    }

    socket.on('update', function(data) {
        console.log("Data received via WebSocket:", data);
        updateTable(data);
        updateChart(data);
    });


    $.ajax({
        url: '/data',
        method: 'GET',
        success: function(data) {
            console.log("Initial data:", data);
            updateTable(data);
            updateChart(data);
        },
        error: function(err) {
            console.error("Failed to fetch initial data", err);
        }
    });

    setInterval(function() {
        $.ajax({
            url: '/data',
            method: 'GET',
            success: function(data) {
                console.log("Updated data:", data);
                updateTable(data);
            },
            error: function(err) {
                console.error("Failed to fetch updated data", err);
            }
        });
    }, 10000);
});
