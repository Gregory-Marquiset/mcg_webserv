document.addEventListener("DOMContentLoaded", () =>
{
	const result = document.getElementById("result");
	const log = document.getElementById("log");

	document.getElementById("httpForm").addEventListener("submit", async (e) =>
	{
		e.preventDefault();
		const url = document.getElementById("url").value;
		const method = document.getElementById("method").value;
		const body = document.getElementById("body").value;
		sendRequest(url, method, body);
	});

	document.getElementById("uploadForm").addEventListener("submit", async (e) =>
	{
		e.preventDefault();
		const fileInput = document.getElementById("fileInput");
		const formData = new FormData();

		for (const file of fileInput.files)
			formData.append("files[]", file);

		const response = await fetch("/upload", {
			method: "POST",
			body: formData
		});

		const text = await response.text();
		result.innerText = `Status: ${response.status} ${response.statusText}\n\n${text}`;
	});

	const postForm = document.getElementById("postCgiForm");
	if (postForm)
	{
		postForm.addEventListener("submit", async (e) =>
		{
			e.preventDefault();
			const filename = document.getElementById("filename-post").value;
			const content = document.getElementById("content-post").value;

			const bodyData = `filename=${encodeURIComponent(filename)}&content=${encodeURIComponent(content)}`;
			const res = await fetch("/cgi_methods/post-handler.cgi", {
				method: "POST",
				headers: { "Content-Type": "application/x-www-form-urlencoded" },
				body: bodyData
			});
			const text = await res.text();
			document.getElementById("cgiPostDeleteResult").textContent = text;
		});
	}

	window.sendRequest = async function (url, method = 'GET', body = null)
	{
		const response = await fetch(url, {
			method: method,
			headers: { 'Content-Type': 'text/plain' },
			body: method === 'POST' ? body : null
		});

		const text = await response.text();
		result.innerText = `Status: ${response.status} ${response.statusText}\n\n${text}`;
		log.innerText = JSON.stringify({
			url: url,
			method: method,
			status: response.status,
			headers: Object.fromEntries(response.headers.entries())
		}, null, 2);
	};

	window.showCookies = function ()
	{
		document.getElementById('cookies').innerText = document.cookie || 'Aucun cookie';
	};
});
