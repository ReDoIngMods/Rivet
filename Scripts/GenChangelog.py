"""
Generate a structured changelog from git commits using conventional commit format.

Parses commits since the latest tag and organizes them by type (feat, fix, refactor, etc.)
Outputs markdown with links to commits on the remote repository.
"""

from git import Repo


def _cap_first(text: str) -> str:
    """Capitalize the first character of the bullet text"""
    return text[:1].upper() + text[1:] if text else text


def _repo_web_url(repo: Repo) -> str | None:
    """Try to determine the web URL of the repository."""

    try:
        remote_url = repo.remotes.origin.url
    except Exception:
        if repo.remotes:
            remote_url = repo.remotes[0].url
        else:
            return None

    remote_url = remote_url.rstrip('/')

    if remote_url.endswith('.git'):
        remote_url = remote_url[:-4]

    # Handle if the user cloned using other protocols

    if remote_url.startswith('git@github.com:'):
        return f"https://github.com/{remote_url.split(':', 1)[1]}"

    if remote_url.startswith('ssh://git@github.com/'):
        return f"https://github.com/{remote_url.split('github.com/', 1)[1]}"

    return remote_url if remote_url.startswith('http') else None


def _format_entry(text: str, commit, repo_url: str | None) -> str:
    """Return a bullet with commit link"""
    if repo_url:
        return f"- {_cap_first(text)} ({repo_url}/commit/{commit.hexsha})"
    return f"- {_cap_first(text)}"

def generate_changelog(repo_path: str) -> str:
    repo = Repo(repo_path)
    
    tags = sorted(repo.tags, key=lambda t: t.commit.committed_date, reverse=True)
    
    if tags:
        from_tag = tags[0].name
        commits = list(repo.iter_commits(f'{from_tag}..HEAD'))
    else:
        commits = list(repo.iter_commits())

    repo_url = _repo_web_url(repo)

    features = []
    fixes = []
    refactors = []
    performance = []
    docs = []
    chores = []
    ci = []
    tests = []
    others = []

    for commit in commits:
        message = commit.message.strip().split('\n')[0]
        subject = message.split('\n\n')[0].strip()
        
        if subject.startswith("feat:"):
            features.append((subject[5:].strip(), commit))
        elif subject.startswith("fix:"):
            fixes.append((subject[4:].strip(), commit))
        elif subject.startswith("refactor:"):
            refactors.append((subject[9:].strip(), commit))
        elif subject.startswith("perf:"):
            performance.append((subject[5:].strip(), commit))
        elif subject.startswith("docs:"):
            docs.append((subject[5:].strip(), commit))
        elif subject.startswith("chore:"):
            chores.append((subject[6:].strip(), commit))
        elif subject.startswith("ci:"):
            ci.append((subject[3:].strip(), commit))
        elif subject.startswith("test:"):
            tests.append((subject[5:].strip(), commit))
        else:
            others.append((subject, commit))

    notes = f"# Changelog\n\n"

    if features:
        notes += "## Features\n"
        for feat, commit in features:
            notes += f"{_format_entry(feat, commit, repo_url)}\n"
        notes += "\n"

    if fixes:
        notes += "## Bug Fixes\n"
        for fix, commit in fixes:
            notes += f"{_format_entry(fix, commit, repo_url)}\n"
        notes += "\n"

    if refactors:
        notes += "## Refactors\n"
        for item, commit in refactors:
            notes += f"{_format_entry(item, commit, repo_url)}\n"
        notes += "\n"

    if performance:
        notes += "## Performance Improvements\n"
        for item, commit in performance:
            notes += f"{_format_entry(item, commit, repo_url)}\n"
        notes += "\n"

    if docs:
        notes += "## Documentation\n"
        for item, commit in docs:
            notes += f"{_format_entry(item, commit, repo_url)}\n"
        notes += "\n"

    if chores:
        notes += "## Chores\n"
        for item, commit in chores:
            notes += f"{_format_entry(item, commit, repo_url)}\n"
        notes += "\n"

    if ci:
        notes += "## CI\n"
        for item, commit in ci:
            notes += f"{_format_entry(item, commit, repo_url)}\n"
        notes += "\n"

    if tests:
        notes += "## Tests\n"
        for item, commit in tests:
            notes += f"{_format_entry(item, commit, repo_url)}\n"
        notes += "\n"

    if others:
        notes += "## Other Changes\n"
        for other, commit in others:
            notes += f"{_format_entry(other, commit, repo_url)}\n"
        notes += "\n"

    return notes


if __name__ == "__main__":
    import os
    import sys
    
    repo_path = os.getcwd()
    
    try:
        notes = generate_changelog(repo_path)
        print(notes)
    except Exception as e:
        print(f"Error generating changelog: {e}", file=sys.stderr)
        sys.exit(1)