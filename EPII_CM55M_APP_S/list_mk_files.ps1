# Get all .mk files and their full paths
Get-ChildItem -Recurse -Filter *.mk | ForEach-Object {
    $filePath = $_.FullName
    Write-Output "File: $filePath"

    # Read the file and extract the comments at the start
    $fileContent = Get-Content -Path $filePath
    $comments = $fileContent | Where-Object { $_ -match '^#' }

    if ($comments) {
        Write-Output "Comments:"
        $comments | ForEach-Object { Write-Output $_ }
    } else {
        Write-Output "No comments found."
    }

    Write-Output ""  # Blank line for better readability
}