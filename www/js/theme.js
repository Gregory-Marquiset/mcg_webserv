function applyTheme( theme )
{
	document.body.classList.remove( 'light-theme', 'dark-theme' );
	document.body.classList.add( theme );
	document.cookie = `theme=${theme}; path=/;`;
}

function toggleTheme()
{
	const	current = document.body.classList.contains( 'light-theme' ) ? 'light-theme' : 'dark-theme';
	const	newTheme = current === 'light-theme' ? 'dark-theme' : 'light-theme';
	applyTheme( newTheme );
}

function getCookieValue( name )
{
	const	match = document.cookie.match( new RegExp( '(^| )' + name + '=([^;]+)' ) );
	return match ? match[2] : null;
}

window.addEventListener('DOMContentLoaded', () =>
{
	const	savedTheme = getCookieValue( 'theme' ) || 'dark-theme';
	applyTheme( savedTheme );
});
