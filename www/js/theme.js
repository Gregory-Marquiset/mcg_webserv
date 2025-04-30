function toggleTheme()
{
	const current = document.body.classList.contains('light-theme') ? 'light-theme' : 'dark-theme';
	const newTheme = current === 'light-theme' ? 'dark-theme' : 'light-theme';
	applyTheme(newTheme);
	window.location.reload();
}

function getCookieValue(name)
{
	const match = document.cookie.match(new RegExp('(^| )' + name + '=([^;]+)'));
	return (match ? match[2] : null);
}

function applyTheme(theme)
{
	document.body.classList.remove('light-theme', 'dark-theme');
	document.body.classList.add(theme);

	const sessionID = getCookieValue("sessionID");
	if (sessionID)
	{
		fetch("/cgi_methods/update-theme.cgi",
		{
			method: "POST",
			headers: { "Content-Type": "application/x-www-form-urlencoded" },
			body: `sessionID=${sessionID}&theme=${theme}`
		});
	}
}

window.addEventListener('DOMContentLoaded', () =>
{
	const tryApplyTheme = () =>
	{
		if (window._SESSION && window._SESSION.theme)
			applyTheme(window._SESSION.theme);
		else
			setTimeout(tryApplyTheme, 50);
	};
	tryApplyTheme();
});
