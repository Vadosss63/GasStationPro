/*
 * This file is part of the GasStationPro project.
 *
 * Copyright (C) 2024 Vadim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "workdirectory.h"

#include "filesystemutilities.h"

WorkDirectory::WorkDirectory(const QString& workDirectory) : workDir(workDirectory) {}

const QString& WorkDirectory::getWorkDirectory() const
{
    createDirIfNeeded(workDir);
    return workDir;
}

std::unique_ptr<WorkDirectory> WorkDirectory::create()
{
    const QString directory = createTmpUniqueDir();
    if (directory.isEmpty())
    {
        return {};
    }

    return std::unique_ptr<WorkDirectory>(new (std::nothrow) WorkDirectory(directory));
}

WorkDirectory::~WorkDirectory()
{
    removeDirectory(workDir);
}
