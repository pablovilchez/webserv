function fetchData() {
	const fetchDataUrl = 'https://jsonplaceholder.typicode.com/todos?_limit=5';
    const postDataUrl = 'http://localhost:8080';
	//http://jsonplaceholder.typicode.com/photos?_start=0&_limit=5
	fetch(fetchDataUrl)
	.then(response => {
		if (response.ok) {
			return response.json();
		} else {
			throw new Error('Failed to fetch data');
		}
	})
	.then(data => {
		console.log('Received data:', data);
		const options = {
			method: 'POST',
			headers: {
				'Content-Type': 'application/json'
			},
			body: JSON.stringify(data)
		};
		return fetch(postDataUrl, options);
	})
	.then(response => {
		if (response.ok) {
			console.log('POST request successful');
		} else {
			throw new Error('Failed to send POST request');
		}
	})
	.catch(error => {
		console.error('Error:', error.message);
	});
}
