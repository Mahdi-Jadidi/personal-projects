document.addEventListener('DOMContentLoaded', () => {
    const eventTypeSelect = document.getElementById('eventType');
    const hiddenEventType = document.getElementById('hiddenEventType');
    const weeklyFields = document.getElementById('weeklyFields');
    const jointFields = document.getElementById('jointFields');
    const form = document.getElementById('eventForm');
    const messageDiv = document.getElementById('message');

    const params = new URLSearchParams(window.location.search);
    if (params.has('success') || params.has('error')) {
        const text = params.get('success') || params.get('error');
        const isSuccess = params.has('success');
        messageDiv.innerText = text;
        messageDiv.style.color = isSuccess ? 'green' : 'red';
        messageDiv.style.opacity = '1';

        setTimeout(() => {
            messageDiv.style.opacity = '0';
        }, 5000);
    }

    eventTypeSelect.addEventListener('change', () => {
        const type = eventTypeSelect.value;
        hiddenEventType.value = type;
        weeklyFields.style.display = type === 'weekly' ? 'block' : 'none';
        jointFields.style.display = type === 'joint' ? 'block' : 'none';
        document.getElementById('date').parentElement.style.display = type === 'weekly' ? 'none' : 'block';
        document.getElementById('duration').parentElement.style.display = type === 'joint' ? 'none' : 'block';
    });

    form.addEventListener('submit', async (e) => {
        e.preventDefault();
        const type = eventTypeSelect.value;
        const formData = new URLSearchParams();

        formData.append('eventType', type);
        formData.append('title', document.getElementById('title').value);
        formData.append('description', document.getElementById('description').value);

        if (type === 'normal') {
            formData.append('date', document.getElementById('date').value);
            formData.append('startTime', document.getElementById('startTime').value);
            formData.append('duration', document.getElementById('duration').value);
        } else if (type === 'weekly') {
            formData.append('startDate', document.getElementById('startDate').value);
            formData.append('endDate', document.getElementById('endDate').value);
            formData.append('startTime', document.getElementById('startTime').value);
            formData.append('duration', document.getElementById('duration').value);

            const checked = document.querySelectorAll('input[name="weekDayCheck"]:checked');
            checked.forEach(cb => formData.append('weekDays', cb.value));
        } else if (type === 'joint') {
            formData.append('date', document.getElementById('date').value);
            formData.append('startTime', document.getElementById('startTime').value);
            formData.append('endTime', document.getElementById('endTime').value);

            const guests = document.getElementById('guests').value.split(',').map(g => g.trim()).filter(Boolean);
            guests.forEach(g => formData.append('guests', g));
        }

        await fetch('/addevent', {
            method: 'POST',
            headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
            body: formData.toString()
        });
    });
});
