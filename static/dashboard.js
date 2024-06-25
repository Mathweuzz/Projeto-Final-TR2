$(document).ready(function() {
    const socket = io.connect('http://127.0.0.1:5000');

    const table = $('#data-table').DataTable({
        order: [[0, 'desc']],
        columnDefs: [
            { className: "text-center", targets: "_all" }
        ]
    });

    let currentPage = 0;
    let pageSize = table.page.len(); 

    function updateTable(data) {
        const pageInfo = table.page.info();
        currentPage = pageInfo.page;
        pageSize = pageInfo.length;
    
        table.clear();
        data.forEach(row => {
            const rowNode = table.row.add([row[0], row[1] +"%", row[2]]).node();
            if (row[1] < 15) {
                $(rowNode).addClass('low-level');
            }
        });
        table.draw();
    
        table.page(currentPage).draw(false);
        table.page.len(pageSize).draw(false);
    }

    const fuelChart = document.getElementById('fuelChart').getContext('2d');
    let chart = null; 

    function updateChart(data) {
        data.reverse();
    
        const labels = data.map(row => moment(row[0]).format('YYYY-MM-DD HH:mm'));
        const levels = data.map(row => row[1]);
        
        if (chart) {
            chart.data.labels = labels;
            chart.data.datasets[0].data = levels;
            chart.update();
        } else {
            chart = new Chart(fuelChart, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [{
                        label: 'Fuel Level',
                        data: levels,
                        borderColor: function(context) {
                            const value = context.dataset.data[context.dataIndex];
                            return value < 15 ? 'red' : 'blue';
                        },
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
                            enabled: true
                        },
                        zoom: {
                            enabled: true
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

        $("#average-level").text("Nível Médio: " + averageLevel.toFixed(2));
        $("#min-level").text("Nível Mínimo: " + minLevel.toFixed(2));
        $("#max-level").text("Nível Máximo: " + maxLevel.toFixed(2));
    }

    function loadTanks() {
        $.ajax({
            url: '/tanks', 
            method: 'GET',
            success: function(data) {
                $('#tank-select').empty(); 
                $('#tank-select').append('<option value="">Todos os Tanques</option>');
                data.forEach(function(tank) {
                    $('#tank-select').append(`<option value="${tank[0]}">${tank[1]}</option>`);
                });
            },
            error: function(err) {
                console.error("Erro ao obter dados dos tanques", err);
            }
        });
    }

    function updateData(tankId = null, startDate = null, endDate = null) {
        let url = '/data';
        let data = {};
        
        if (tankId) {
            url = `/data/filtered/${tankId}`;
        }
        
        if (startDate && endDate) {
            data.start_date = startDate;
            data.end_date = endDate;
        }

        console.log(`Making AJAX request to ${url} with data:`, data);

        $.ajax({
            url: url,
            method: 'GET',
            data: data,
            success: function(response) {
                console.log("Data received from server:", response);
                updateTable(response);
                updateChart(response);
                updateStats(response);
            },
            error: function(err) {
                console.error("Erro ao obter dados", err);
            }
        });
    }

    socket.on('connect', function() {
        console.log('Connected to WebSocket'); 
    });

    socket.on('update', function(data) {
        console.log("Data received via WebSocket:", data);
        updateTable(data);
        updateChart(data);
        updateStats(data); 
    });

    loadTanks();
    updateData(); 

    $('#tank-select').change(function() {
        const selectedTankId = $(this).find(":selected").val();
        const startDate = $("#start-date").val();
        const endDate = $("#end-date").val();

        updateData(selectedTankId, startDate, endDate);
    });

    $("#filter-form").submit(function(event) {
        event.preventDefault();
    
        const startDate = $("#start-date").val();
        const endDate = $("#end-date").val();
        const selectedTankOption = $('#tank-select').find(':selected');
        const tankId = selectedTankOption.val() ? selectedTankOption.val().match(/\d+/)[0] : null;
    
        const requestData = {
            start_date: startDate,
            end_date: endDate
        };
        if (tankId) {
            requestData.tank_id = tankId;
        }
    
        console.log("Making AJAX request to /data with data:", requestData);
    
        $.ajax({
            url: '/data/filtered', 
            data: requestData,
            success: function(data) {
                console.log("Data received from server:", data);
                updateTable(data);
                updateChart(data);
                updateStats(data); 
            },
            error: function(err) {
                console.error("Erro ao obter dados filtrados", err);
            }
        });
    });

});
