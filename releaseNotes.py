"""
Script to generate release notes for Rivet.

It requires gitpython to be installed:
    pip install gitpython

This is possible since Rivet uses git and conventional commits for commit messages.
"""

from git import Repo

def generate_release_notes(repo_path: str) -> str:
    repo = Repo(repo_path)
    
    # Get all tags sorted by version
    tags = sorted(repo.tags, key=lambda t: t.commit.committed_date, reverse=True)
    
    # Get commits since the last tag, or all commits if no tags exist
    if tags:
        from_tag = tags[0].name
        commits = list(repo.iter_commits(f'{from_tag}..HEAD'))
        to_tag = "Unreleased"
    else:
        commits = list(repo.iter_commits())
        to_tag = "Initial Release"

    features = []
    fixes = []
    others = []

    for commit in commits:
        # Use only the first line of the commit message (subject)
        subject = commit.message.strip().split("\n", 1)[0].strip()

        if subject.startswith("feat:"):
            features.append(subject[5:].strip())
        elif subject.startswith("fix:"):
            fixes.append(subject[4:].strip())
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
        notes = generate_release_notes(repo_path)
        print(notes)
    except Exception as e:
        print(f"Error generating release notes: {e}", file=sys.stderr)
        sys.exit(1)
