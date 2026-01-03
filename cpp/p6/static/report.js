document.getElementById('reportForm').addEventListener('submit', function(e) {
    const from = document.getElementById('from').value;
    const to = document.getElementById('to').value;
    const errorDiv = document.getElementById('errorMsg');
    errorDiv.textContent = '';

    if (!from || !to) {
        e.preventDefault();
        errorDiv.textContent = 'Please enter both start and end dates.';
        return;
    }
    if (to < from) {
        e.preventDefault();
        errorDiv.textContent = 'End date cannot be earlier than start date.';
        return;
    }
});
