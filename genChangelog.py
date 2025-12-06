"""
Script to generate a changeloig for Rivet.

It requires gitpython to be installed:
    pip install gitpython

This is possible since Rivet uses git and conventional commits for commit messages.
"""

from git import Repo

def generate_changelog(repo_path: str) -> str:
    repo = Repo(repo_path)
    
    # Get all tags sorted by version
    tags = sorted(repo.tags, key=lambda t: t.commit.committed_date, reverse=True)
    
    # Get commits since the last tag, or all commits if no tags exist
    if tags:
        from_tag = tags[0].name
        commits = list(repo.iter_commits(f'{from_tag}..HEAD'))
    else:
        commits = list(repo.iter_commits())

    features = []
    fixes = []
    refactors = []
    performance = []
    docs = []
    chores = []
    tests = []
    others = []

    for commit in commits:
        # Get only the first line of the commit message (subject)
        message = commit.message.strip().split('\n')[0]
        # Extract just the commit subject (up to first double newline or end)
        subject = message.split('\n\n')[0].strip()
        
        if subject.startswith("feat:"):
            features.append(subject[5:].strip())
        elif subject.startswith("fix:"):
            fixes.append(subject[4:].strip())
        elif subject.startswith("refactor:"):
            refactors.append(subject[9:].strip())
        elif subject.startswith("perf:"):
            performance.append(subject[5:].strip())
        elif subject.startswith("docs:"):
            docs.append(subject[5:].strip())
        elif subject.startswith("chore:"):
            chores.append(subject[6:].strip())
        elif subject.startswith("test:"):
            tests.append(subject[5:].strip())
        else:
            others.append(subject)

    notes = f"# Changelog\n\n"

    if features:
        notes += "## Features\n"
        for feat in features:
            notes += f"- {feat}\n"
        notes += "\n"

    if fixes:
        notes += "## Bug Fixes\n"
        for fix in fixes:
            notes += f"- {fix}\n"
        notes += "\n"

    if refactors:
        notes += "## Refactors\n"
        for item in refactors:
            notes += f"- {item}\n"
        notes += "\n"

    if performance:
        notes += "## Performance Improvements\n"
        for item in performance:
            notes += f"- {item}\n"
        notes += "\n"

    if docs:
        notes += "## Documentation\n"
        for item in docs:
            notes += f"- {item}\n"
        notes += "\n"

    if chores:
        notes += "## Chores\n"
        for item in chores:
            notes += f"- {item}\n"
        notes += "\n"

    if tests:
        notes += "## Tests\n"
        for item in tests:
            notes += f"- {item}\n"
        notes += "\n"

    if others:
        notes += "## Other Changes\n"
        for other in others:
            notes += f"- {other}\n"
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
