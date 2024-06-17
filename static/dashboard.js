$(document).ready(function() {
    const socket = io.connect('http://127.0.0.1:5000');

    const table = $('#data-table').DataTable({
        order: [[0, 'desc']]
    });

    let currentPage = 0;
    let pageSize = table.page.len(); 

    function updateTable(data) {
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

    socket.on('update', function(data) {
        console.log("Data received via WebSocket:", data);
        updateTable(data);
        updateChart(data);
        updateStats(data); // Adiciona a atualização das estatísticas
    });

    $.ajax({
        url: '/data',
        method: 'GET',
        success: function(data) {
            console.log("Initial data:", data);
            updateTable(data);
            updateChart(data);
            updateStats(data); // Adiciona a atualização das estatísticas
        },
        error: function(err) {
            console.error("Failed to fetch initial data", err);
        }
    });

    const fuelChart = document.getElementById('fuelChart').getContext('2d');
    let chart = null; 

    function updateChart(data) {
        data.reverse();

        const labels = data.map(row => moment(row[0]).format('YYYY-MM-DD HH:mm'));
        const levels = data.map(row => row[1]);

        if (chart) {
            chart.data.labels = labels;
            chart.data.datasets[0].data = levels;
            chart.options.plugins.tooltip.enabled = true; // Habilita o tooltip
            chart.options.plugins.zoom.enabled = true; // Habilita o zoom
            chart.update();
        } else {
            chart = new Chart(fuelChart, {
                type: 'line',
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
                            beginAtZero: true
                        }
                    },
                    plugins: {
                        tooltip: {
                            enabled: true // Habilita o tooltip
                        },
                        zoom: {
                            enabled: true // Habilita o zoom
                        }
                    }
                }
            });
        }
    }

    function updateStats(data) {
        const levels = data.map(row => row[1]);
        const averageLevel = levels.reduce((sum, level) => sum + level, 0) / levels.length;
        const minLevel = Math.min(...levels);
        const maxLevel = Math.max(...levels);

        // Exibe as estatísticas no HTML
        $("#average-level").text("Nível Médio: " + averageLevel.toFixed(2));
        $("#min-level").text("Nível Mínimo: " + minLevel.toFixed(2));
        $("#max-level").text("Nível Máximo: " + maxLevel.toFixed(2));
    }

    $("#filter-form").submit(function(event) {
        event.preventDefault();

        const startDate = $("#start-date").val();
        const endDate = $("#end-date").val();

        $.ajax({
            url: '/data/filtered',
            data: {
                start_date: startDate,
                end_date: endDate
            },
            success: function(data) {
                updateTable(data);
                updateChart(data);
                updateStats(data); // Adiciona a atualização das estatísticas
            },
            error: function(err) {
                console.error("Erro ao obter dados filtrados", err);
            }
        });
    });
});